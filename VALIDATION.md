# 验证记录

验证日期：2026-09-14（Asia/Shanghai）。正式默认模型仍是 Qwen3.5-4B，
本机测试只临时使用已有的 **Qwen3.5-2B** 缓存，没有下载或复制其他模型权重。

## 实测环境与方法

- GPU：NVIDIA GeForce RTX 4070 SUPER，12GB，WSL2/Linux。
- Python 3.11.13；PyTorch 2.10.0+cu128；LLaMA-Factory 0.9.5；
  Transformers 5.6.0；PEFT 0.18.1；bitsandbytes 0.49.2。
- 模型 revision：`15852e8c16360a2fea060d615a32b45270f8a8fc`。
- 全部243条训练数据；临时 batch=1、梯度累积=1、长度1024、2次参数更新。
- 默认 LoRA rank=8、alpha=16、SDPA、梯度检查点；8,409,600个可训练参数，372个LoRA张量。
- 执行 `bash tests/run_gpu.sh`，三种模式顺序训练、重新加载适配器生成32 tokens，
  然后从 checkpoint-1 恢复到累计 step 2。

## GPU 结果

| 模式 | 两步 loss | 训练峰值 allocated / reserved | 4-bit 线性层 | 训练 / 重载 / 续训 |
| --- | --- | --- | --- | --- |
| BF16 LoRA（none） | 0.86013 → 0.64542 | 7.18 / 7.86 GiB | 0 | 全部通过 |
| NF4 + BF16 | 0.91354 → 0.66929 | 4.84 / 5.50 GiB | 284 | 全部通过 |
| FP4 + BF16 | 0.96385 → 0.73169 | 4.84 / 5.50 GiB | 284 | 全部通过 |

三种模式的 loss、梯度范数和适配器张量均为有限值；每次新训练及恢复训练均确认
186个适配器张量发生更新。视觉部分没有可训练参数。运行时检查了实际量化层、
量化格式和计算 dtype，避免仅修改配置但未启用量化。

显存为 PyTorch 记录的训练峰值，不包括其他应用占用。这里的两步 loss 只用于验证
计算流程，不能用于比较量化模式的最终质量。

本机完整证据位于：

```text
output/local-validation-20260913-161426/
  none/
  nf4/
  fp4/
```

各目录的 `model_report.json`、`metrics.jsonl`、`training_verification.json`、
`reload_verification.json` 与 `diagnostics/resume-*/` 保存对应证据。
上述大体积运行输出被 Git 忽略，迁移项目后可用验证脚本重新生成。

## 数据与代码检查

- `uv run --locked pytest -q`：**35 passed**。
- 全局 `ruff check`：通过。
- 新工作树中的三个数据文件均与原工作树逐字节一致；原工作树 Git 状态校验未变。
- 数据维持原来的 CRLF；`.gitattributes` 禁止转换数据文件的换行符，以便跨机器保持哈希。
- 所有243条样本通过提示词掩码和有效答案 token 检查，实际训练没有重复混入 messages 副本。

2B 分词器下原始长度：最短864、中位2508、P95为3734、最长4395 tokens。
本机验证长度1024截断234/243条（96.30%），每条仍至少有724个有效答案 tokens。
因此1024仅用于流程验证；正式默认长度仍是4096，并会重新报告实际截断情况。

| 文件 | SHA-256 |
| --- | --- |
| dataset_info.json | `ddd932586850b19318383e2f2738b5844624beb2e05dae6f5713f359e331ad4c` |
| deepseek_solved_finetuning.jsonl | `b14a866d37767c3ec7df582ff4d6c24ea75f1769ed03e82de2bfb4127ecd631a` |
| train_alpaca.jsonl | `a191bf39bc77c55f2744309e599ec40c5b9997a51894ed24093d86e76568c9bb` |

## 尚未实测的范围

当前无法访问目标4090，**Qwen3.5-4B / 4090 24GB / 长度4096尚未做GPU实测**。
目标机三种配置的训练、重载和续训验收命令见 README。

FP16、FP32、FlashAttention-2、其他模型尺寸、验证集评估及自选优化器尚未分别做GPU实测；
对应配置解析与约束已检查。未执行完整3轮训练或独立代码正确性评估。
