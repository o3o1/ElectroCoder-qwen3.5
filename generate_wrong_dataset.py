import os
import json
import re
import subprocess
import asyncio
from openai import AsyncOpenAI
import httpx

# ==========================================
# 1. 核心环境配置：100% 指向本地 LM Studio (Qwen)
# ==========================================
os.environ["NO_PROXY"] = "127.0.0.1,localhost"
os.environ["no_proxy"] = "127.0.0.1,localhost"

# 修复：降低客户端超时 + 限制连接池，防止内存堆积
client = AsyncOpenAI(
    base_url="http://127.0.0.1:1234/v1",
    api_key="dummy",
    timeout=httpx.Timeout(60.0),
    http_client=httpx.AsyncClient(
        limits=httpx.Limits(max_keepalive_connections=1, max_connections=1),
        timeout=httpx.Timeout(60.0)
    )
)


# ==========================================
# 2. 核心功能函数
# ==========================================
def extract_c_code(text: str) -> str:
    """提取 C 代码块"""
    match = re.search(r'```(?:c|cpp|C)?\s*(.*?)\s*```', text, re.DOTALL)
    if match:
        return match.group(1).strip()
    return text.strip()


def run_evaluation_cmd(command: str):
    """
    修复版：
    1. 逐行读取输出，避免一次性 read() 导致 MemoryError
    2. 严格限制输出长度，保护内存
    3. 强制编码为 utf-8，避免崩溃
    """
    exe_match = re.search(r'-o\s+(.*?\.exe)', command)
    if exe_match:
        exe_path = exe_match.group(1).strip()
        exe_name = os.path.basename(exe_path)
        subprocess.run(f"taskkill /F /IM {exe_name} >nul 2>nul", shell=True)

    try:
        # 核心修复：使用 Popen 逐行读取，不一次性加载全部输出
        process = subprocess.Popen(
            command,
            shell=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            encoding='utf-8',
            errors='replace'
        )

        # 逐行读取 + 长度硬限制，彻底解决 MemoryError
        stdout_lines = []
        stderr_lines = []
        max_output_lines = 30  # 最多保留30行，防止内存爆炸
        max_line_length = 200  # 每行最长200字符

        # 读 stdout
        for idx, line in enumerate(process.stdout):
            if idx >= max_output_lines:
                break
            line = line.strip()[:max_line_length]
            stdout_lines.append(line)

        # 读 stderr
        for idx, line in enumerate(process.stderr):
            if idx >= max_output_lines:
                break
            line = line.strip()[:max_line_length]
            stderr_lines.append(line)

        # 等待进程结束
        returncode = process.wait(timeout=15)

        stdout_str = "\n".join(stdout_lines)
        stderr_str = "\n".join(stderr_lines)

        if returncode == 0:
            return True, stdout_str
        else:
            return False, f"STDERR:\n{stderr_str}\nSTDOUT:\n{stdout_str}"

    except subprocess.TimeoutExpired:
        if exe_match:
            subprocess.run(f"taskkill /F /IM {exe_name} >nul 2>nul", shell=True)
        return False, "运行超时(超过15秒)！代码存在死循环或等待输入。"
    except Exception as e:
        return False, f"系统异常: {str(e)}"


# ==========================================
# 3. 电力电子考点（完全保留你原来的内容）
# ==========================================
def load_pe_topics(file_path="pe_topics.txt"):
    if not os.path.exists(file_path):
        print(f"【错误】未找到考点文件: {file_path}")
        return []
    topics = []
    with open(file_path, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if line:
                topics.append(line)
    return topics

# 外部文件加载考点，替代原来超大列表
PE_TOPICS = load_pe_topics()


# ==========================================
# 4. 主流程：本地 Qwen 遍历摸底与自我修正
# ==========================================
async def evaluate_qwen_baseline():
    HARD_TASKS_FILE = "qwen_hard_tasks_for_finetuning.jsonl"
    PROGRESS_FILE = "qwen_processed_topics.txt"

    processed_topics = set()
    if os.path.exists(PROGRESS_FILE):
        with open(PROGRESS_FILE, "r", encoding="utf-8") as f:
            for line in f:
                processed_topics.add(line.strip())

    total = len(PE_TOPICS)
    remaining = total - len(processed_topics)

    print(f"🚀 开始评估【本地 Qwen 基线模型】的专业代码生成与报错自纠正能力！")
    print(f"📚 总考点数: {total} 个，待测: {remaining} 个\n" + "=" * 60)

    for i, topic in enumerate(PE_TOPICS):
        if topic in processed_topics:
            continue

        print(f"\n▶️[进度 {i + 1}/{total}] 正在测试本地 Qwen 对考点: 【{topic}】 的掌握程度...")

        instruction = (
            f"任务需求：请用C语言编写一段关于【{topic}】的控制算法或代码骨架。\n"
            f"要求：\n"
            f"1. 必须包含完整的 main 函数和简单的 printf 测试打印。\n"
            f"2. 严禁引用单片机特定的硬件头文件(如 stm32f10x.h)，缺失的寄存器请自行使用宏定义(mock)。\n"
            f"3. 代码必须能用标准 GCC 编译通过。"
        )

        file_path = f"./sandbox/eval_qwen.c"
        check_cmd = f"gcc -Wall {file_path} -o ./sandbox/eval_qwen.exe && .\\sandbox\\eval_qwen.exe"

        messages = [
            {
                "role": "system",
                "content": "你是一个电力电子专家。请完成用户的编程任务，你只需输出完整的 C 语言代码，千万不要解释，必须把代码放在 ```c 和 ``` 之间。"
            },
            {
                "role": "user",
                "content": instruction
            }
        ]

        is_solved = False

        for turn in range(1, 6):
            print(f"  ⏳ [本地 Qwen] 第 {turn} 次思考与生成代码...")
            try:
                response = await asyncio.wait_for(
                    client.chat.completions.create(
                        model="qwen2.5-coder-7b-instruct",
                        messages=messages,
                        temperature=0.2,
                        max_tokens=1200  # 进一步降低token，减少内存占用
                    ),
                    timeout=90.0
                )

                reply_text = response.choices[0].message.content
                messages.append({"role": "assistant", "content": reply_text})

                code = extract_c_code(reply_text)
                if not code.strip():
                    print("  ⚠️ 警告：Qwen 没有输出代码块格式。")

                os.makedirs(os.path.dirname(file_path), exist_ok=True)
                with open(file_path, "w", encoding="utf-8") as f:
                    f.write(code)

                is_success, run_output = run_evaluation_cmd(check_cmd)

                if is_success:
                    if turn == 1:
                        print("  ✅ 1次通过！Qwen 竟然直接掌握了这个知识点！")
                    else:
                        print(f"  ✅ 修正通过！（历经 {turn} 次尝试）。Qwen 成功自我纠正！")
                    is_solved = True
                    break
                else:
                    truncated_output = run_output.strip()
                    if len(truncated_output) > 600:
                        truncated_output = truncated_output[:600] + "\n...(报错已截断)..."

                    print(f"  ❌ 运行失败。GCC反馈:\n>> {truncated_output[:150]} ...")

                    error_feedback = (
                        f"你生成的代码报错了：\n{truncated_output}\n"
                        f"请仔细分析原因，找出 Bug 并修正它。\n"
                        f"【注意】：如果是找不到头文件的错误，请直接删除该 #include 语句，改为手动 mock 寄存器！如果是 lvalue 报错，说明你赋值语句的左边不是变量！\n"
                        f"请直接输出修正后的完整 C 语言代码，必须放在 ```c 块中。"
                    )
                    messages.append({"role": "user", "content": error_feedback})

            except asyncio.TimeoutError:
                print("  ⏰[系统警报] 本地模型生成超时！强制打断本轮！")
                break
            except Exception as e:
                print(f"  ⚠️ LM Studio 请求异常: {str(e)}")
                break

        if not is_solved:
            print(f"  🚫 历经 {turn} 次修正失败或卡死！【{topic}】已触及 Qwen 天花板，记录至微调错题本！")
            hard_task_record = {
                "topic": topic,
                "instruction": instruction,
                "failed_reason": "Failed to compile/run after 5 corrections or API hang."
            }
            with open(HARD_TASKS_FILE, "a", encoding="utf-8") as f:
                f.write(json.dumps(hard_task_record, ensure_ascii=False) + "\n")

        with open(PROGRESS_FILE, "a", encoding="utf-8") as f:
            f.write(topic + "\n")

    print("\n🏆 评估与“错题本”整理完成！你可以去检查 qwen_hard_tasks_for_finetuning.jsonl 了。")


if __name__ == "__main__":
    asyncio.run(evaluate_qwen_baseline())