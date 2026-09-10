import os
import json
import re
import subprocess
import asyncio
from openai import AsyncOpenAI
import httpx

# ==========================================
# 1. 环境配置：指向 DeepSeek API
# ==========================================
# 你的 DeepSeek API Key (请注意保管好你的 API Key)
DEEPSEEK_API_KEY = os.environ.get("DEEPSEEK_API_KEY", "REDACTED_API_KEY")

client = AsyncOpenAI(
    base_url="https://api.deepseek.com",
    api_key=DEEPSEEK_API_KEY,
    timeout=httpx.Timeout(60.0),
    http_client=httpx.AsyncClient(
        limits=httpx.Limits(max_keepalive_connections=5, max_connections=10),
        timeout=httpx.Timeout(60.0)
    )
)

# ==========================================
# 2. 核心功能函数 (沙盒运行与提取)
# ==========================================
def extract_c_code(text: str) -> str:
    """提取 C 代码块（支持大模型忘记写闭合 ``` 的情况）"""
    match = re.search(r'```(?:c|cpp|C)?\s*(.*?)(?:```|$)', text, re.DOTALL)
    if match:
        return match.group(1).strip()
    return text.strip()

def run_evaluation_cmd(command: str):
    """
    沙盒运行环境（终极防卡死版）：
    将输出重定向到文件，彻底避免 OS Pipe 缓冲区满导致的死锁，
    同时避免 scanf 等待输入导致同步阻塞读取卡死。
    """
    exe_name = ""
    exe_match = re.search(r'-o\s+(.*?\.exe)', command)
    if exe_match:
        exe_path = exe_match.group(1).strip()
        exe_name = os.path.basename(exe_path)
        # 运行前确保旧的进程已清理
        subprocess.run(f"taskkill /F /IM {exe_name} >nul 2>nul", shell=True)

    out_file = "./sandbox/stdout.txt"
    err_file = "./sandbox/stderr.txt"
    os.makedirs("./sandbox", exist_ok=True)

    try:
        # 1. 启动子进程，输出直接灌入物理文件，Python不参与管道读取
        with open(out_file, "w", encoding="utf-8") as f_out, open(err_file, "w", encoding="utf-8") as f_err:
            process = subprocess.Popen(
                command,
                shell=True,
                stdout=f_out,
                stderr=f_err
            )
            # Python 在这里安心等待，如果超过 15 秒没运行完，直接抛出异常
            returncode = process.wait(timeout=15)

    except subprocess.TimeoutExpired:
        # 发生死循环或 scanf 等待输入，强制杀掉并返回
        process.kill()
        if exe_name:
            subprocess.run(f"taskkill /F /IM {exe_name} >nul 2>nul", shell=True)
        return False, "运行超时(超过15秒)！代码存在死循环(while(1))或使用了scanf等待键盘输入导致阻塞。"
    except Exception as e:
        return False, f"系统异常: {str(e)}"

    # 2. 安全读取前 30 行文件内容，防止极长输出导致内存溢出
    stdout_lines = []
    try:
        with open(out_file, "r", encoding="utf-8", errors="replace") as f_out:
            for idx, line in enumerate(f_out):
                if idx >= 30: break
                stdout_lines.append(line.strip()[:200])
    except: pass

    stderr_lines = []
    try:
        with open(err_file, "r", encoding="utf-8", errors="replace") as f_err:
            for idx, line in enumerate(f_err):
                if idx >= 30: break
                stderr_lines.append(line.strip()[:200])
    except: pass

    stdout_str = "\n".join(stdout_lines)
    stderr_str = "\n".join(stderr_lines)

    # 3. 错误诊断分析
    if returncode == 0:
        return True, stdout_str
    else:
        # 捕获由于内存越界、除以零导致的程序静默崩溃 (没有 stderr 但 Exit code 不为 0)
        if not stderr_str.strip() and returncode != 0:
            crash_msg = f"程序发生异常崩溃 (Exit Code: {returncode})！可能发生了内存越界访问(Segfault)、除以零错误，或者 main 函数返回了非零值。\n"
            return False, f"STDERR:\n{crash_msg}\nSTDOUT:\n{stdout_str}"
        else:
            return False, f"STDERR:\n{stderr_str}\nSTDOUT:\n{stdout_str}"

# ==========================================
# 3. 读取“错题本”数据
# ==========================================
def load_hard_tasks(file_path="qwen_hard_tasks_for_finetuning.jsonl"):
    if not os.path.exists(file_path):
        print(f"【错误】未找到错题本文件: {file_path}")
        return []
    tasks = []
    with open(file_path, "r", encoding="utf-8") as f:
        for line in f:
            if line.strip():
                tasks.append(json.loads(line))
    return tasks

# ==========================================
# 4. 主流程：DeepSeek 接管纠错并生成微调数据集
# ==========================================
async def generate_finetuning_dataset():
    INPUT_TASKS_FILE = "qwen_hard_tasks_for_finetuning.jsonl"
    OUTPUT_DATASET_FILE = "deepseek_solved_finetuning.jsonl"
    PROGRESS_FILE = "deepseek_processed_topics.txt"

    SYSTEM_PROMPT = "你是一个顶级的电力电子软件专家。请完成用户的编程任务，你只需输出完整的 C 语言代码，千万不要解释，必须把代码放在 ```c 和 ``` 之间。"

    tasks = load_hard_tasks(INPUT_TASKS_FILE)
    if not tasks:
        return

    # 读取断点续传进度
    processed_topics = set()
    if os.path.exists(PROGRESS_FILE):
        with open(PROGRESS_FILE, "r", encoding="utf-8") as f:
            for line in f:
                processed_topics.add(line.strip())

    total = len(tasks)
    remaining = total - len(processed_topics)

    print(f"🚀 开始调用【DeepSeek】处理错题本并生成微调数据集！")
    print(f"📚 错题总数: {total} 个，待处理: {remaining} 个\n" + "=" * 60)

    for i, task in enumerate(tasks):
        topic = task["topic"]
        instruction = task["instruction"]

        if topic in processed_topics:
            continue

        print(f"\n▶️[进度 {i + 1}/{total}] DeepSeek 正在攻克难题: 【{topic}】...")

        file_path = f"./sandbox/eval_deepseek.c"
        check_cmd = f"gcc -Wall {file_path} -o ./sandbox/eval_deepseek.exe && .\\sandbox\\eval_deepseek.exe"

        messages = [
            {"role": "system", "content": SYSTEM_PROMPT},
            {"role": "user", "content": instruction}
        ]

        is_solved = False
        final_correct_code = ""

        # DeepSeek 进行最多 10 轮的编译报错自修正
        for turn in range(1, 10):
            print(f"  ⏳ [DeepSeek] 第 {turn} 次思考与生成代码...")
            try:
                # 终极修复：给网络请求套上 120 秒的绝对强制超时，防止底层 TCP/HTTP 请求假死锁死整个进程
                response = await asyncio.wait_for(
                    client.chat.completions.create(
                        model="deepseek-chat",
                        messages=messages,
                        temperature=0.2,
                        max_tokens=4096
                    ),
                    timeout=120.0
                )

                reply_text = response.choices[0].message.content
                messages.append({"role": "assistant", "content": reply_text})

                code = extract_c_code(reply_text)
                if not code.strip():
                    print("  ⚠️ 警告：DeepSeek 没有输出有效的代码块。")

                os.makedirs(os.path.dirname(file_path), exist_ok=True)
                with open(file_path, "w", encoding="utf-8") as f:
                    f.write(code)

                is_success, run_output = run_evaluation_cmd(check_cmd)

                if is_success:
                    if turn == 1:
                        print("  ✅ 1次通过！DeepSeek 秒杀了这道题！")
                    else:
                        print(f"  ✅ 修正通过！（历经 {turn} 次尝试）。DeepSeek 成功自纠正！")
                    is_solved = True
                    final_correct_code = code
                    break
                else:
                    truncated_output = run_output.strip()
                    if len(truncated_output) > 600:
                        truncated_output = truncated_output[:600] + "\n...(报错已截断)..."

                    print(f"  ❌ 运行失败。GCC反馈:\n>> {truncated_output[:150]} ...")

                    # 增强版的错误回投提示词，告知模型排查崩溃
                    error_feedback = (
                        f"你生成的代码报错或运行异常了：\n{truncated_output}\n"
                        f"请仔细分析原因，找出 Bug 并修正它。\n"
                        f"【注意】：\n"
                        f"1. 如果是找不到头文件的错误，请直接删除该 #include 语句，改为手动 mock 寄存器！\n"
                        f"2. 如果是 Exit Code 非0 的程序崩溃，请重点检查：指针是否为空或未初始化、数组是否越界、是否存在除以 0、死循环，并确保 main 最终 return 0;\n"
                        f"请直接输出修正后的完整 C 语言代码，必须放在 ```c 和 ``` 之间。"
                    )
                    messages.append({"role": "user", "content": error_feedback})

            except asyncio.TimeoutError:
                print("  ⏰ [系统警报] DeepSeek API 响应超时 (超过 120 秒)！正在自动重试本轮...")
                # 超时不断开，直接重试当前 turn (不会破坏循环)
                continue
            except Exception as e:
                print(f"  ⚠️ DeepSeek API 请求或沙盒异常: {str(e)}")
                break

        # 如果最终解题成功，整理成标准的大模型微调格式
        if is_solved:
            finetuning_record = {
                "messages": [
                    {"role": "system", "content": SYSTEM_PROMPT},
                    {"role": "user", "content": instruction},
                    {"role": "assistant", "content": f"```c\n{final_correct_code}\n```"}
                ]
            }
            # 追加写入最终的微调数据集文件
            with open(OUTPUT_DATASET_FILE, "a", encoding="utf-8") as f:
                f.write(json.dumps(finetuning_record, ensure_ascii=False) + "\n")
            print("  💾 已将正确答案保存至微调数据集。")
        else:
            print(f"  🚫 历经 {turn} 次修正依然失败！此题已放弃。")

        # 记录处理进度，防止意外中断时重复跑题
        with open(PROGRESS_FILE, "a", encoding="utf-8") as f:
            f.write(topic + "\n")

    print(f"\n🏆 错题本重塑完毕！你可以使用 {OUTPUT_DATASET_FILE} 配合 LLaMA-Factory 对本地 Qwen 进行微调了！")

if __name__ == "__main__":
    asyncio.run(generate_finetuning_dataset())