import json

file_path = "pe_dataset.jsonl"

try:
    with open(file_path, "r", encoding="utf-8") as f:
        lines = f.readlines()

    print(f"\n🎉 当前数据集共包含 {len(lines)} 条成功验证的数据！\n")

    for i, line in enumerate(lines):
        # 解析每一行的 JSON 数据
        data = json.loads(line.strip())

        print(f"========== 🏆 第 {i + 1} 条有效数据 ==========")
        print(f"【原始任务 (Instruction)】:\n{data['instruction']}\n")
        print(f"【验证通过的代码 (Output)】:\n{data['output']}")
        print("============================================\n")

except FileNotFoundError:
    print(f"❌ 找不到文件 {file_path}，请确认数据是否已经成功生成。")