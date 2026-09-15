"""Reload a saved training adapter and run a short, deterministic generation."""

import argparse
import json
import os
from pathlib import Path

from train import dump_json, factory_dtype, register_text_template


def main():
    cli = argparse.ArgumentParser(description=__doc__)
    cli.add_argument("--run-dir", required=True, type=Path)
    cli.add_argument("--gpu", type=int, default=0)
    cli.add_argument("--max-new-tokens", type=int, default=32)
    cli.add_argument("--prompt", default="请用 C 语言实现一个有输出限幅的 PI 控制器。")
    options = cli.parse_args()
    if options.gpu < 0 or options.max_new_tokens < 1:
        cli.error("gpu must be nonnegative and max-new-tokens must be positive")
    directory = options.run_dir.resolve()
    config = json.loads((directory / "effective_config.json").read_text())["options"]
    os.environ["CUDA_VISIBLE_DEVICES"] = str(options.gpu)
    os.environ["HF_HUB_OFFLINE"] = "1" if config["offline"] else "0"
    os.environ["TOKENIZERS_PARALLELISM"] = "false"

    import torch
    from llamafactory.data import get_template_and_fix_tokenizer
    from llamafactory.hparams import get_infer_args
    from llamafactory.model import load_model, load_tokenizer
    from safetensors.torch import load_file

    weights = load_file(str(directory / "adapter_model.safetensors"))
    if not weights or any(not torch.isfinite(t).all() for t in weights.values()):
        raise RuntimeError("Adapter tensors are missing or contain non-finite values")
    register_text_template()
    from train import TEXT_TEMPLATE

    args = {
        "model_name_or_path": config["model"],
        "model_revision": config["revision"],
        "adapter_name_or_path": str(directory),
        "template": TEXT_TEMPLATE,
        "infer_dtype": {"bf16": "bfloat16", "fp16": "float16", "fp32": "float32"}[
            config["precision"]
        ],
        "flash_attn": config["attention"],
        "quantization_bit": None if config["quantization"] == "none" else 4,
        "quantization_type": config["quantization"] if config["quantization"] != "none" else "nf4",
        "double_quantization": config["double_quantization"],
    }
    model_args, data_args, finetuning_args, _ = get_infer_args(args)
    model_args.device_map = {"": torch.device("cuda:0")}
    tokenizer = load_tokenizer(model_args)["tokenizer"]
    template = get_template_and_fix_tokenizer(tokenizer, data_args)
    with factory_dtype(getattr(torch, args["infer_dtype"])):
        model = load_model(tokenizer, model_args, finetuning_args, is_trainable=False)
    prompt, _ = template.encode_oneturn(
        tokenizer,
        [
            {"role": "user", "content": options.prompt},
            {"role": "assistant", "content": ""},
        ],
    )
    inputs = torch.tensor([prompt], device="cuda")
    with torch.inference_mode():
        generated = model.generate(
            input_ids=inputs,
            attention_mask=torch.ones_like(inputs),
            do_sample=False,
            max_new_tokens=options.max_new_tokens,
            eos_token_id=tokenizer.eos_token_id,
            pad_token_id=tokenizer.pad_token_id,
        )
    tokens = generated[0, len(prompt) :].tolist()
    if not tokens:
        raise RuntimeError("Reloaded adapter did not generate any tokens")
    report = {
        "adapter_tensors": len(weights),
        "all_adapter_values_finite": True,
        "generated_tokens": len(tokens),
        "prompt": options.prompt,
        "text": tokenizer.decode(tokens, skip_special_tokens=False),
        "gpu": torch.cuda.get_device_name(0),
    }
    dump_json(directory / "reload_verification.json", report)
    print(json.dumps(report, ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
