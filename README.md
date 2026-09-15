# ElectroCoder Qwen3.5 训练

使用现有 243 条电力电子代码样本，对 Qwen3.5 做纯文本 LoRA / QLoRA 微调。
正式默认目标是 **Linux x86_64 / WSL2 + 单张 RTX 4090 24GB**，模型为
`Qwen/Qwen3.5-4B`。模型尺寸和参数都可以更换；更大模型的显存需求需要单独实测。

## 安装与启动

需要可用的 NVIDIA 驱动和 uv（>=0.8.15）。在本工作树根目录执行：

```bash
nvidia-smi
uv sync --locked
uv run --locked train.py --dry-run
uv run --locked train.py --config configs/nf4.yaml
```

也可用 `bash train.sh ...`，它会先切换到项目根目录。首次安装下载 Python 包和
CUDA 运行库；Hub 模型首次训练会下载权重。已有模型使用本地 HF 目录和 `--offline`：

```bash
uv run --locked train.py --model /absolute/path/to/Qwen3.5-4B --offline
```

输入必须是原始、未量化的 Hugging Face 格式权重；不接受 GGUF 或预量化模型。
切换 Qwen3.5 尺寸直接修改 `--model`。默认按 Qwen3.5 的直接回答格式训练，
不增加推理过程、不制作新数据。最终保存 LoRA 适配器，推理时仍需要对应基础模型。

依赖锁定 Python 3.11.13、LLaMA-Factory 0.9.5、Transformers 5.6.0、PEFT 0.18.1、
PyTorch 2.10.0 / torchvision 0.25.0 / torchaudio 2.10.0（CUDA 12.8）。
其余精确版本及下载校验值见 `uv.lock`。请使用 `--locked`，不要单独升级训练依赖。
uv 缓存权限不足时使用有权限的终端运行原命令，不修改缓存位置。

## BF16、NF4 和 FP4

**计算精度与权重量化是独立设置。** `--precision` 支持 `bf16/fp16/fp32`；
`--quantization` 支持 `none/nf4/fp4`。FP4 在这里指 bitsandbytes 4-bit 权重量化，
与 NF4 一样用于 QLoRA。默认 NF4 + BF16，并开启双重量化。

```bash
# NF4 权重 + BF16 计算（默认）
uv run --locked train.py --config configs/nf4.yaml
# 不量化基础模型，BF16 LoRA
uv run --locked train.py --config configs/bf16.yaml
# FP4 权重 + BF16 计算
uv run --locked train.py --config configs/fp4.yaml
# 显式选择其他计算精度或关闭双重量化
uv run --locked train.py --quantization fp4 --precision fp16 --no-double-quantization
```

三个配置只指定模型和精度差异，其他值来自统一默认配置。
所有模式均只训练语言 LoRA；视觉塔、视觉投影器和基础权重保持冻结。
项目复用官方 `qwen3_5_nothink` 文本格式，并使用本地纯文本模板别名
`electrocoder_qwen3_5_text_nothink` 关闭图像/视频插件，避免纯文本批次要求图像处理器
或构造虚拟图像。因此只有文本文件的本地缓存也可以使用。该别名由本项目入口注册，
运行原生 `llamafactory-cli` 时不会自动注册；请通过 `train.py` 启动。

## 常用参数

优先级为 **内置默认值 < YAML < 显式命令行参数**。YAML 使用下表参数的下划线形式，
例如 `batch_size`；未知键会报错。YAML 的数据、输出、续训路径相对 YAML 文件所在目录；
以 `./`、`../`、`/` 或 `~` 开头的模型路径同理。CLI 路径相对当前终端目录。
未指定数据目录时固定使用项目自己的数据目录。

| 参数 | 默认值 / 说明 |
| --- | --- |
| `--model`、`--revision` | `Qwen/Qwen3.5-4B`、`main`；支持固定模型提交 |
| `--dataset-dir`、`--dataset` | 项目数据目录、`deepseek_ee_code` |
| `--batch-size`、`--gradient-accumulation-steps` | 1、8；单卡有效 batch=8 |
| `--cutoff-len` | 4096，含提示词和答案；超长样本按框架规则截断 |
| `--epochs`、`--max-steps` | 3、-1；正数 max-steps 优先于 epochs |
| `--learning-rate`、`--lr-scheduler` | 5e-5、cosine；还支持 linear/constant/constant_with_warmup |
| `--warmup-ratio`、`--weight-decay`、`--max-grad-norm` | 0.03、0、1 |
| `--optimizer` | adamw_torch；可选 adamw_torch_fused/paged_adamw_8bit |
| `--lora-rank`、`--lora-alpha`、`--lora-dropout` | 8、16、0 |
| `--lora-target` | all，由框架选择语言线性层；可传逗号分隔模块名 |
| `--gradient-checkpointing` / `--no-gradient-checkpointing` | 默认开启 |
| `--attention` | sdpa；支持 eager/fa2，fa2 需另装兼容的 flash-attn |
| `--val-size`、`--seed` | 0、42；验证集比例在 [0,1)，启用后每轮计算 eval loss |
| `--logging-steps`、`--save-steps`、`--save-total-limit` | 1、50、2 |
| `--gpu` | 0，物理 GPU 编号；入口仅支持单卡 |
| `--output-dir` | 自动生成 output 下的独立目录 |
| `--resume-from-checkpoint` | 显式选择完整 checkpoint 目录 |

默认关闭 packing，仅答案 token 参与损失。数据目录中两个 JSONL 都保留，实际训练只使用
数据注册表指向的 `train_alpaca.jsonl`，不会将 messages 格式的副本重复加入训练。
可以修改已支持参数的 YAML，或用 `uv run --locked train.py --help` 查看完整选项。

```bash
uv run --locked train.py --batch-size 2 --gradient-accumulation-steps 4 \
  --cutoff-len 4096 --epochs 3 --learning-rate 0.00005 --val-size 0.1
```

## 检查、本机测试与目标机验收

`--dry-run` 只验证配置、数据和输出路径并打印最终设置，不导入训练库、不创建输出。
`--check-only` 进一步检查模型配置、分词、答案掩码和 CUDA 前向/反向，不加载模型权重。
`--smoke-test` 从新目录训练两次参数更新，每次保存 checkpoint；不自动缩短序列或换模型。

本机已下载的 **Qwen3.5-2B** 缓存用于临时验证，正式默认配置保持 4B/4096：

```bash
QWEN_TEST_MODEL=/mnt/wsl/data/cache/huggingface/hub/models--Qwen--Qwen3.5-2B/snapshots/15852e8c16360a2fea060d615a32b45270f8a8fc
uv run --locked train.py --model "$QWEN_TEST_MODEL" --offline --check-only \
  --batch-size 1 --gradient-accumulation-steps 1 --cutoff-len 1024
uv run --locked train.py --model "$QWEN_TEST_MODEL" --offline --smoke-test \
  --batch-size 1 --gradient-accumulation-steps 1 --cutoff-len 1024 \
  --quantization nf4 --precision bf16 --output-dir output/local-nf4
uv run --locked verify_adapter.py --run-dir output/local-nf4
```

同样使用 `--quantization none` 和 `--quantization fp4`，并换用新的输出目录，即可验证
另外两种模式。模型缓存按只读路径加载，不复制进项目，不下载替代模型。
长度1024只验证流程，可能截断大量代码；正式运行前查看数据报告再决定长度。

也可直接运行 `bash tests/run_gpu.sh`：自动查找缓存中的 Qwen3.5-2B，找不到完整权重时再找
0.8B，全程离线，顺序验证三种模式、适配器加载和从中间 checkpoint 恢复。
脚本接受一个已有小模型目录作为可选参数。

4090 目标机使用 4B 和正式长度4096逐一运行以下命令，分别令 MODE 为 `nf4`、`bf16`、`fp4`：

```bash
QWEN_TEST_MODE=nf4
uv run --locked train.py --config "configs/$QWEN_TEST_MODE.yaml" --smoke-test \
  --output-dir "output/4090-$QWEN_TEST_MODE"
uv run --locked verify_adapter.py --run-dir "output/4090-$QWEN_TEST_MODE"
uv run --locked train.py --config "configs/$QWEN_TEST_MODE.yaml" --max-steps 2 --save-steps 1 \
  --resume-from-checkpoint "output/4090-$QWEN_TEST_MODE/checkpoint-1"
```

检查有限 loss/梯度、实际 LoRA 更新、视觉参数冻结、适配器重新加载与生成、从 step 2
中间 checkpoint 继续完成原定步数，以及各模式显存峰值。当前实际验证范围见 `VALIDATION.md`。

## 保存、续训与诊断

每次运行保存 `effective_config.json`、`environment.json`、`data_report.json` 和
`console.log`；训练另有逐步指标、模型参数检查、参数更新/显存报告、适配器和 checkpoints。
数据报告包括所有数据文件的 SHA-256、样本数、原始 token 长度分位数、截断比例和有效答案检查。
日志每60秒记录线程栈，帮助定位首步卡顿。堆栈快照本身不代表死锁。

已有非空输出目录必须显式续训。传入完整 `checkpoint-N`，省略 output-dir 时自动使用其父目录。
续训会检查模型、量化、LoRA、关键优化参数和数据哈希；只含适配器的目录不能恢复优化器状态。
`--max-steps` 表示**累计总步数**，不是增加的步数。继续上面的本机测试：

```bash
uv run --locked train.py --model "$QWEN_TEST_MODEL" --offline \
  --batch-size 1 --gradient-accumulation-steps 1 --cutoff-len 1024 \
  --quantization nf4 --precision bf16 --max-steps 2 --save-steps 1 \
  --resume-from-checkpoint output/local-nf4/checkpoint-1
```

续训诊断保存在原输出目录的 diagnostics 子目录中，原始配置和日志保留。
已有 checkpoint 恢复原优化器/调度器状态；建议从中间 checkpoint 恢复原训练计划。
已完成 cosine 计划的最后 checkpoint 学习率可能为0，直接延长步数不能保证立即更新参数；
需要改变学习率计划的实验应另开新训练。
BF16 不支持、未知模型、数据错误或无有效答案时会报错，不静默降精度或跳样本。
OOM 时可减少长度/batch、启用梯度检查点或改用量化模式；程序不自动改参数重跑。

## 开发检查

```bash
ruff check .
uv run --locked pytest -q
```

测试覆盖配置覆盖关系、精度/量化组合、非法参数、数据检查、输出保护及续训兼容。
本项目不包含数据生成 API、旧 sandbox、模型权重、原虚拟环境或历史训练结果。
