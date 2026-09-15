"""Single-GPU Qwen3.5 text SFT using LLaMA-Factory's data and training pipeline."""

import argparse
import copy
import faulthandler
import hashlib
import importlib.metadata
import json
import math
import os
import platform
import re
import sys
import traceback
from contextlib import contextmanager, redirect_stderr, redirect_stdout
from datetime import datetime
from pathlib import Path
from unittest.mock import patch

import yaml

ROOT = Path(__file__).resolve().parent
DEFAULTS = {
    "model": "Qwen/Qwen3.5-4B",
    "revision": "main",
    "dataset_dir": str(ROOT / "data"),
    "dataset": "deepseek_ee_code",
    "precision": "bf16",
    "quantization": "nf4",
    "double_quantization": True,
    "batch_size": 1,
    "gradient_accumulation_steps": 8,
    "cutoff_len": 4096,
    "epochs": 3.0,
    "max_steps": -1,
    "learning_rate": 5e-5,
    "lr_scheduler": "cosine",
    "warmup_ratio": 0.03,
    "weight_decay": 0.0,
    "max_grad_norm": 1.0,
    "optimizer": "adamw_torch",
    "lora_rank": 8,
    "lora_alpha": 16,
    "lora_dropout": 0.0,
    "lora_target": "all",
    "gradient_checkpointing": True,
    "attention": "sdpa",
    "val_size": 0.0,
    "seed": 42,
    "logging_steps": 1,
    "save_steps": 50,
    "save_total_limit": 2,
    "gpu": 0,
    "output_dir": None,
    "resume_from_checkpoint": None,
    "offline": False,
}
CHOICES = {
    "precision": ("bf16", "fp16", "fp32"),
    "quantization": ("none", "nf4", "fp4"),
    "attention": ("sdpa", "eager", "fa2"),
    "optimizer": ("adamw_torch", "adamw_torch_fused", "paged_adamw_8bit"),
    "lr_scheduler": ("cosine", "linear", "constant", "constant_with_warmup"),
}
BOOL_KEYS = {"double_quantization", "gradient_checkpointing", "offline"}
INT_KEYS = {
    "batch_size",
    "gradient_accumulation_steps",
    "cutoff_len",
    "max_steps",
    "lora_rank",
    "lora_alpha",
    "seed",
    "logging_steps",
    "save_steps",
    "save_total_limit",
    "gpu",
}
FLOAT_KEYS = {
    "epochs",
    "learning_rate",
    "warmup_ratio",
    "weight_decay",
    "max_grad_norm",
    "lora_dropout",
    "val_size",
}
RESUME_KEYS = {
    "model",
    "revision",
    "dataset",
    "precision",
    "quantization",
    "double_quantization",
    "cutoff_len",
    "lora_rank",
    "lora_alpha",
    "lora_dropout",
    "lora_target",
    "seed",
    "val_size",
    "batch_size",
    "gradient_accumulation_steps",
    "optimizer",
    "learning_rate",
    "lr_scheduler",
    "warmup_ratio",
    "weight_decay",
    "max_grad_norm",
}
TEXT_TEMPLATE = "electrocoder_qwen3_5_text_nothink"


def timestamp():
    return datetime.now().strftime("%Y%m%d-%H%M%S-%f")


def dump_json(path, value):
    Path(path).write_text(json.dumps(value, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def parser():
    cli = argparse.ArgumentParser(description=__doc__)
    cli.add_argument("--config", type=Path, help="YAML file; explicit CLI options win")
    modes = cli.add_mutually_exclusive_group()
    modes.add_argument(
        "--dry-run", action="store_true", help="Validate/print configuration; no GPU or output"
    )
    modes.add_argument(
        "--check-only",
        action="store_true",
        help="Check data, tokenization and CUDA, without loading weights",
    )
    modes.add_argument(
        "--smoke-test",
        action="store_true",
        help="Two optimizer steps, with a checkpoint after each",
    )
    for name in DEFAULTS:
        kwargs = {"default": argparse.SUPPRESS}
        if name in BOOL_KEYS:
            kwargs["action"] = argparse.BooleanOptionalAction
        elif name in INT_KEYS:
            kwargs["type"] = int
        elif name in FLOAT_KEYS:
            kwargs["type"] = float
        if name in CHOICES:
            kwargs["choices"] = CHOICES[name]
        cli.add_argument("--" + name.replace("_", "-"), **kwargs)
    return cli


def resolve_config(argv=None):
    options = vars(parser().parse_args(argv))
    config = DEFAULTS.copy()
    config_file = options.pop("config")
    if config_file:
        raw = yaml.safe_load(config_file.read_text(encoding="utf-8"))
        if not isinstance(raw, dict):
            raise ValueError("YAML must contain a mapping of training options")
        unknown = raw.keys() - DEFAULTS.keys()
        if unknown:
            raise ValueError(f"Unknown YAML options: {sorted(unknown)}")
        for key in ("dataset_dir", "output_dir", "resume_from_checkpoint"):
            if raw.get(key) is not None:
                raw[key] = str(
                    (config_file.resolve().parent / Path(raw[key]).expanduser()).resolve()
                )
        if isinstance(raw.get("model"), str) and raw["model"].startswith((".", "~", "/")):
            raw["model"] = str(
                (config_file.resolve().parent / Path(raw["model"]).expanduser()).resolve()
            )
        config.update(raw)
    mode = "train"
    for flag in ("dry_run", "check_only", "smoke_test"):
        if options.pop(flag):
            mode = flag
    config.update(options)
    validate_config(config)
    model = Path(config["model"]).expanduser()
    if model.exists():
        config["model"] = str(model.resolve())
    elif config["model"].startswith((".", "/", "~")):
        raise ValueError(f"Local model directory does not exist: {model}")
    config["dataset_dir"] = str(Path(config["dataset_dir"]).expanduser().resolve())
    if mode == "smoke_test":
        if config["resume_from_checkpoint"]:
            raise ValueError(
                "Use --max-steps for resume; --smoke-test always starts a new two-step run"
            )
        config.update(max_steps=2, save_steps=1)
    checkpoint = config["resume_from_checkpoint"]
    if checkpoint:
        checkpoint = Path(checkpoint).expanduser().resolve()
        config["resume_from_checkpoint"] = str(checkpoint)
    if config["output_dir"]:
        output = Path(config["output_dir"]).expanduser().resolve()
    elif checkpoint:
        output = checkpoint.parent
    else:
        name = re.sub(r"[^A-Za-z0-9_.-]", "_", Path(config["model"]).name)
        output = ROOT / "output" / f"{name}-{timestamp()}"
    config["output_dir"] = str(output)
    return config, mode


def validate_config(config):
    for key in BOOL_KEYS:
        if type(config[key]) is not bool:
            raise ValueError(f"{key} must be true or false")
    for key in INT_KEYS:
        if type(config[key]) is not int:
            raise ValueError(f"{key} must be an integer")
        if key not in {"max_steps", "seed", "gpu"} and config[key] < 1:
            raise ValueError(f"{key} must be positive")
    for key in FLOAT_KEYS:
        if isinstance(config[key], bool) or not isinstance(config[key], (int, float)):
            raise ValueError(f"{key} must be a finite number")
        if not math.isfinite(config[key]):
            raise ValueError(f"{key} must be finite")
    for key, values in CHOICES.items():
        if config[key] not in values:
            raise ValueError(f"{key} must be one of {values}")
    for key in ("model", "revision", "dataset_dir", "dataset", "lora_target"):
        if not isinstance(config[key], str) or not config[key].strip():
            raise ValueError(f"{key} must be a nonempty string")
    for key in ("output_dir", "resume_from_checkpoint"):
        if config[key] is not None and (
            not isinstance(config[key], str) or not config[key].strip()
        ):
            raise ValueError(f"{key} must be a nonempty path or null")
    if config["cutoff_len"] < 2 or config["gpu"] < 0 or config["seed"] < 0:
        raise ValueError("cutoff_len must be >=2; gpu and seed must be nonnegative")
    if config["max_steps"] != -1 and config["max_steps"] < 1:
        raise ValueError("max_steps must be -1 or positive")
    for key in ("epochs", "learning_rate", "max_grad_norm"):
        if config[key] <= 0:
            raise ValueError(f"{key} must be positive")
    for key in ("warmup_ratio", "lora_dropout", "val_size"):
        if not 0 <= config[key] < 1:
            raise ValueError(f"{key} must be in [0, 1)")
    if config["weight_decay"] < 0:
        raise ValueError("weight_decay must be nonnegative")
    if "," in config["dataset"]:
        raise ValueError(
            "Select one local Alpaca dataset; comma-separated mixtures are not supported"
        )


def read_data(config):
    directory = Path(config["dataset_dir"])
    registry_path = directory / "dataset_info.json"
    registry = json.loads(registry_path.read_text(encoding="utf-8-sig"))
    if config["dataset"] not in registry:
        raise ValueError(f"Unknown dataset: {config['dataset']}")
    entry = registry[config["dataset"]]
    if entry.get("formatting", "alpaca") != "alpaca" or entry.get("ranking"):
        raise ValueError("This entrypoint requires a local, non-ranking Alpaca dataset")
    columns = entry.get("columns", {})
    if set(columns) - {"prompt", "query", "response"}:
        raise ValueError(
            "Only instruction/input/output columns are supported by this text SFT entrypoint"
        )
    fields = {
        key: columns.get(key, default)
        for key, default in (("prompt", "instruction"), ("query", "input"), ("response", "output"))
    }
    path = (directory / entry["file_name"]).resolve()
    if not path.is_relative_to(directory.resolve()):
        raise ValueError("Dataset file must be inside dataset_dir")
    rows = []
    for number, line in enumerate(path.read_text(encoding="utf-8-sig").splitlines(), 1):
        if not line.strip():
            raise ValueError(f"Empty JSONL line {number}")
        row = json.loads(line)
        if not isinstance(row, dict):
            raise ValueError(f"JSONL line {number} must contain an object")
        for role, field in fields.items():
            value = row.get(field)
            if not isinstance(value, str) or (role != "query" and not value.strip()):
                raise ValueError(f"Invalid {field!r} at line {number}")
        rows.append({key: row[field] for key, field in fields.items()})
    if not rows:
        raise ValueError("Dataset is empty")
    if config["val_size"] and math.ceil(len(rows) * config["val_size"]) >= len(rows):
        raise ValueError("Validation split leaves no training samples")
    hashes = {
        str(p.relative_to(directory)): hashlib.sha256(p.read_bytes()).hexdigest()
        for p in sorted(directory.rglob("*"))
        if p.is_file()
    }
    return rows, {"samples": len(rows), "training_file": str(path), "sha256": hashes}


def validate_output(config, data_info):
    output = Path(config["output_dir"])
    checkpoint = config["resume_from_checkpoint"]
    if not checkpoint:
        if output.exists() and (not output.is_dir() or any(output.iterdir())):
            raise ValueError(
                "Output directory must be new/empty; explicitly resume or choose another path"
            )
        return
    checkpoint = Path(checkpoint)
    if checkpoint.parent != output:
        raise ValueError(
            "Checkpoint must be directly inside output_dir; omit --output-dir to infer it"
        )
    for name in ("trainer_state.json", "optimizer.pt", "scheduler.pt", "adapter_config.json"):
        if not (checkpoint / name).is_file():
            raise ValueError(f"Incomplete training checkpoint: missing {name}")
    if not any(
        (checkpoint / n).is_file() for n in ("adapter_model.safetensors", "adapter_model.bin")
    ):
        raise ValueError("Checkpoint is missing adapter weights")
    state = json.loads((checkpoint / "trainer_state.json").read_text())
    if config["max_steps"] > 0 and config["max_steps"] <= state["global_step"]:
        raise ValueError(
            "max_steps must exceed the checkpoint's global_step (it is a total, not extra steps)"
        )
    previous = json.loads((output / "effective_config.json").read_text())
    changed = [key for key in sorted(RESUME_KEYS) if previous["options"][key] != config[key]]
    if changed:
        raise ValueError(f"Resume configuration differs: {', '.join(changed)}")
    old_data = json.loads((output / "data_report.json").read_text())
    if old_data["sha256"] != data_info["sha256"]:
        raise ValueError("Dataset changed since the original run; start a new output directory")


def factory_config(config):
    rename = {
        "model": "model_name_or_path",
        "revision": "model_revision",
        "batch_size": "per_device_train_batch_size",
        "epochs": "num_train_epochs",
        "lr_scheduler": "lr_scheduler_type",
        "warmup_ratio": "warmup_steps",
        "optimizer": "optim",
        "attention": "flash_attn",
    }
    omitted = {"precision", "quantization", "gradient_checkpointing", "gpu", "offline"}
    result = {rename.get(k, k): v for k, v in config.items() if k not in omitted}
    result.update(
        stage="sft",
        do_train=True,
        finetuning_type="lora",
        template=TEXT_TEMPLATE,
        fp16=config["precision"] == "fp16",
        bf16=config["precision"] == "bf16",
        quantization_method="bnb",
        quantization_bit=None if config["quantization"] == "none" else 4,
        quantization_type="nf4" if config["quantization"] == "none" else config["quantization"],
        disable_gradient_checkpointing=not config["gradient_checkpointing"],
        use_reentrant_gc=False,
        train_on_prompt=False,
        packing=False,
        freeze_vision_tower=True,
        freeze_multi_modal_projector=True,
        freeze_language_model=False,
        report_to="none",
        logging_first_step=True,
        logging_strategy="steps",
        save_strategy="steps",
        dataloader_num_workers=0,
        dataloader_pin_memory=False,
        preprocessing_num_workers=1,
        overwrite_cache=False,
        do_eval=config["val_size"] > 0,
        eval_strategy="epoch" if config["val_size"] else "no",
        per_device_eval_batch_size=1,
        prediction_loss_only=True,
        seed=config["seed"],
        data_seed=config["seed"],
        disable_tqdm=True,
    )
    return result


def register_text_template():
    """Keep the official text format; disable the unused image/video plugin for text SFT."""
    from llamafactory.data.mm_plugin import get_mm_plugin
    from llamafactory.data.template import TEMPLATES

    template = copy.deepcopy(TEMPLATES["qwen3_5_nothink"])
    template.mm_plugin = get_mm_plugin(name="base")
    TEMPLATES[TEXT_TEMPLATE] = template


@contextmanager
def factory_dtype(dtype):
    """Make the selected dtype explicit when the pinned factory loads an HF model."""
    from llamafactory.model import loader

    original_patch = loader.patch_config

    def patch_dtype(model_config, tokenizer, model_args, init_kwargs, is_trainable):
        original_patch(model_config, tokenizer, model_args, init_kwargs, is_trainable)
        # LLaMA-Factory 0.9.5 passes torch_dtype='auto'; Transformers v5's dtype
        # takes precedence, including for FP16/FP32 and adapter reloads.
        init_kwargs["dtype"] = dtype

    with patch.object(loader, "patch_config", patch_dtype):
        yield


def check_environment(config):
    import torch

    if int(os.environ.get("WORLD_SIZE", "1")) != 1:
        raise ValueError("This entrypoint supports single-GPU training only")
    if not torch.cuda.is_available():
        raise RuntimeError("CUDA is unavailable; check the NVIDIA driver and Linux/WSL GPU access")
    if config["precision"] == "bf16" and not torch.cuda.is_bf16_supported():
        raise RuntimeError("GPU does not support BF16; explicitly choose fp16 or fp32")
    if config["attention"] == "fa2":
        try:
            importlib.metadata.version("flash-attn")
        except importlib.metadata.PackageNotFoundError as exc:
            raise ValueError(
                "fa2 requires a compatible flash-attn installation; default SDPA needs none"
            ) from exc
    dtype = getattr(
        torch, {"bf16": "bfloat16", "fp16": "float16", "fp32": "float32"}[config["precision"]]
    )
    probe = torch.ones((128, 128), device="cuda", dtype=dtype, requires_grad=True)
    (probe @ probe).float().mean().backward()
    torch.cuda.synchronize()
    del probe
    torch.cuda.empty_cache()
    free, total = torch.cuda.mem_get_info()
    return {
        "python": sys.version,
        "platform": platform.platform(),
        "gpu": torch.cuda.get_device_name(0),
        "gpu_capability": torch.cuda.get_device_capability(0),
        "free_gpu_gib": free / 2**30,
        "total_gpu_gib": total / 2**30,
        "torch_cuda": torch.version.cuda,
        "packages": dict(
            sorted((d.metadata["Name"], d.version) for d in importlib.metadata.distributions())
        ),
    }


def check_tokens(rows, model_args, data_args):
    from llamafactory.data import get_template_and_fix_tokenizer
    from llamafactory.data.processor.supervised import SupervisedDatasetProcessor
    from llamafactory.model import load_config, load_tokenizer

    config = load_config(model_args)
    if config.model_type not in {"qwen3_5", "qwen3_5_moe", "qwen3_5_text", "qwen3_5_moe_text"}:
        raise ValueError(f"Expected Qwen3.5 architecture, got {config.model_type!r}")
    if getattr(config, "quantization_config", None):
        raise ValueError(
            "Use original unquantized HF weights; this script applies the selected quantization"
        )
    module = load_tokenizer(model_args)
    tokenizer = module["tokenizer"]
    template = get_template_and_fix_tokenizer(tokenizer, data_args)
    encoder = SupervisedDatasetProcessor(template, tokenizer, None, data_args)
    lengths, answer_lengths = [], []
    special_ids = set(tokenizer.all_special_ids)
    for number, row in enumerate(rows, 1):
        prompt = [
            {"role": "user", "content": "\n".join(x for x in (row["prompt"], row["query"]) if x)}
        ]
        response = [{"role": "assistant", "content": row["response"]}]
        pairs = template.encode_multiturn(tokenizer, prompt + response, "", "")
        lengths.append(sum(len(source) + len(target) for source, target in pairs))
        ids, labels = encoder._encode_data_example(prompt, response, "", "", [], [], [])
        active = [label for label in labels if label != -100]
        if not active or not any(label not in special_ids for label in active):
            raise ValueError(f"No effective answer tokens after truncation at sample {number}")
        if (
            not labels
            or labels[0] != -100
            or any(
                label != -100 and label != token for token, label in zip(ids, labels, strict=True)
            )
        ):
            raise ValueError(f"Invalid prompt/answer loss mask at sample {number}")
        answer_lengths.append(len(active))
    ordered = sorted(lengths)
    report = {
        "raw_tokens_min": ordered[0],
        "raw_tokens_median": ordered[len(ordered) // 2],
        "raw_tokens_p95": ordered[math.ceil(len(ordered) * 0.95) - 1],
        "raw_tokens_max": ordered[-1],
        "truncated_samples": sum(n > data_args.cutoff_len for n in lengths),
        "truncated_ratio": sum(n > data_args.cutoff_len for n in lengths) / len(lengths),
        "min_answer_tokens_after_truncation": min(answer_lengths),
        "prompt_mask_checked_samples": len(rows),
        "model_type": config.model_type,
        "resolved_model_revision": getattr(config, "_commit_hash", None),
    }
    print("[data] " + json.dumps(report, ensure_ascii=False), flush=True)
    return report


def training_callback(config, diagnostics):
    import torch
    from bitsandbytes.nn import Linear4bit
    from transformers import TrainerCallback

    class Audit(TrainerCallback):
        def __init__(self):
            self.initial = {}

        def on_train_begin(self, args, state, control, model=None, **kwargs):
            trainable = [(n, p) for n, p in model.named_parameters() if p.requires_grad]
            if not trainable or any("lora_" not in n or ".visual." in n for n, _ in trainable):
                raise RuntimeError("Only language LoRA parameters may be trainable")
            self.initial = {n: p.detach().float().cpu().clone() for n, p in trainable}
            quantized = [m for m in model.modules() if isinstance(m, Linear4bit)]
            if (config["quantization"] != "none") != bool(quantized):
                raise RuntimeError("Loaded model quantization does not match requested mode")
            wanted = {"bf16": torch.bfloat16, "fp16": torch.float16, "fp32": torch.float32}[
                config["precision"]
            ]
            if quantized and any(
                m.compute_dtype != wanted or m.weight.quant_type != config["quantization"]
                for m in quantized
            ):
                raise RuntimeError("4-bit format or compute dtype does not match requested mode")
            base_dtypes = {
                p.dtype
                for n, p in model.named_parameters()
                if "language_model" in n
                and "lora_" not in n
                and p.is_floating_point()
                and p.ndim == 2
                and "embed_tokens" not in n
            }
            if not quantized and wanted not in base_dtypes:
                raise RuntimeError("Language weight dtype does not match requested precision")
            report = {
                "trainable_parameters": sum(p.numel() for _, p in trainable),
                "trainable_tensors": len(trainable),
                "vision_frozen": True,
                "linear4bit_modules": len(quantized),
                "parameter_dtypes": sorted({str(p.dtype) for p in model.parameters()}),
                "requested_compute_dtype": str(wanted),
                "language_matrix_dtypes": sorted(str(d) for d in base_dtypes),
                "start_global_step": state.global_step,
            }
            dump_json(diagnostics / "model_report.json", report)
            print("[model] " + json.dumps(report), flush=True)
            torch.cuda.reset_peak_memory_stats()

        def on_log(self, args, state, control, logs=None, **kwargs):
            for key in ("loss", "grad_norm", "eval_loss", "train_loss"):
                if key in (logs or {}) and not math.isfinite(float(logs[key])):
                    raise RuntimeError(f"Non-finite {key} at step {state.global_step}")
            record = {
                "step": state.global_step,
                **(logs or {}),
                "peak_gpu_gib": torch.cuda.max_memory_allocated() / 2**30,
            }
            with (diagnostics / "metrics.jsonl").open("a", encoding="utf-8") as stream:
                stream.write(json.dumps(record, ensure_ascii=False) + "\n")
            print("[progress] " + json.dumps(record), flush=True)

        def on_train_end(self, args, state, control, model=None, **kwargs):
            changed = []
            for name, param in model.named_parameters():
                if name in self.initial:
                    value = param.detach().float().cpu()
                    if not torch.isfinite(value).all():
                        raise RuntimeError(f"Non-finite adapter parameter: {name}")
                    if not torch.equal(value, self.initial[name]):
                        changed.append(name)
            if not changed:
                raise RuntimeError("Training finished without any LoRA parameter update")
            dump_json(
                diagnostics / "training_verification.json",
                {
                    "global_step": state.global_step,
                    "changed_adapter_tensors": len(changed),
                    "all_adapter_values_finite": True,
                    "peak_allocated_gib": torch.cuda.max_memory_allocated() / 2**30,
                    "peak_reserved_gib": torch.cuda.max_memory_reserved() / 2**30,
                },
            )

    return Audit()


def execute(config, mode, rows, data_info, diagnostics):
    import torch
    from transformers import set_seed

    environment = check_environment(config)
    dump_json(diagnostics / "environment.json", environment)
    print(
        f"[environment] GPU={environment['gpu']}; free={environment['free_gpu_gib']:.2f} GiB",
        flush=True,
    )
    from llamafactory.hparams import get_train_args
    from llamafactory.train.sft.workflow import run_sft

    register_text_template()
    lf_config = factory_config(config)
    args = get_train_args(lf_config)
    model_args, data_args, training_args, finetuning_args, generating_args = args
    dtype = {"bf16": torch.bfloat16, "fp16": torch.float16, "fp32": torch.float32}[
        config["precision"]
    ]
    model_args.compute_dtype = dtype
    set_seed(config["seed"])
    data_info.update(check_tokens(rows, model_args, data_args))
    dump_json(diagnostics / "data_report.json", data_info)
    if mode == "check_only":
        print(
            "[success] Data, tokenizer, prompt masks and CUDA checked; "
            "model weights were not loaded."
        )
        return

    with factory_dtype(dtype):
        run_sft(
            model_args,
            data_args,
            training_args,
            finetuning_args,
            generating_args,
            callbacks=[training_callback(config, diagnostics)],
        )
    print("[success] Training completed; adapter and checkpoints saved.", flush=True)


class Tee:
    def __init__(self, console, logfile):
        self.console, self.logfile = console, logfile

    def write(self, value):
        self.console.write(value)
        self.logfile.write(value)
        self.flush()
        return len(value)

    def flush(self):
        self.console.flush()
        self.logfile.flush()

    def __getattr__(self, key):
        return getattr(self.console, key)


def main(argv=None):
    try:
        config, mode = resolve_config(argv)
        rows, data_info = read_data(config)
        validate_output(config, data_info)
    except (ValueError, OSError, KeyError, TypeError, yaml.YAMLError) as exc:
        print(f"[error] {exc}", file=sys.stderr)
        return 2
    effective = {"options": config, "mode": mode, "llamafactory": factory_config(config)}
    if mode == "dry_run":
        print(json.dumps({**effective, "data": data_info}, indent=2, ensure_ascii=False))
        return 0
    os.environ["CUDA_VISIBLE_DEVICES"] = str(config["gpu"])
    os.environ["HF_HUB_OFFLINE"] = "1" if config["offline"] else "0"
    os.environ["HF_DATASETS_OFFLINE"] = "1" if config["offline"] else "0"
    os.environ["TOKENIZERS_PARALLELISM"] = "false"
    os.environ.setdefault("HF_HUB_DISABLE_TELEMETRY", "1")
    output = Path(config["output_dir"])
    diagnostics = (
        output / "diagnostics" / f"resume-{timestamp()}"
        if config["resume_from_checkpoint"]
        else output
    )
    diagnostics.mkdir(parents=True, exist_ok=True)
    dump_json(diagnostics / "effective_config.json", effective)
    dump_json(diagnostics / "data_report.json", data_info)
    print(f"Run output: {output}\nDiagnostics: {diagnostics}", flush=True)
    with (diagnostics / "console.log").open("w", encoding="utf-8", buffering=1) as logfile:
        with redirect_stdout(Tee(sys.stdout, logfile)), redirect_stderr(Tee(sys.stderr, logfile)):
            faulthandler.dump_traceback_later(60, repeat=True, file=logfile)
            try:
                execute(config, mode, rows, data_info, diagnostics)
            except KeyboardInterrupt:
                print("[interrupted] Partial diagnostics/checkpoints retained.")
                return 130
            except Exception as exc:
                traceback.print_exc()
                if "out of memory" in str(exc).lower():
                    print(
                        "[hint] Reduce --cutoff-len/--batch-size, enable gradient checkpointing, "
                        "or choose NF4/FP4. Settings were not changed."
                    )
                return 1
            finally:
                faulthandler.cancel_dump_traceback_later()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
