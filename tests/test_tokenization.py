"""CPU integration check using an existing Qwen3.5 tokenizer, never downloading one."""

from pathlib import Path

import pytest

import train


def test_native_config_matches_pinned_factory_arguments():
    from dataclasses import fields

    from llamafactory.extras.constants import QuantizationMethod
    from llamafactory.hparams import (
        DataArguments,
        FinetuningArguments,
        GeneratingArguments,
        ModelArguments,
    )
    from llamafactory.hparams.training_args import TrainingArguments

    allowed = {
        field.name
        for cls in (
            DataArguments,
            FinetuningArguments,
            GeneratingArguments,
            ModelArguments,
            TrainingArguments,
        )
        for field in fields(cls)
    }
    config, _ = train.resolve_config([])
    assert not set(train.factory_config(config)) - allowed
    assert train.factory_config(config)["quantization_method"] == QuantizationMethod.BNB


def cached_tokenizer():
    from huggingface_hub.constants import HF_HUB_CACHE

    repo = Path(HF_HUB_CACHE) / "models--Qwen--Qwen3.5-2B" / "snapshots"
    snapshots = sorted(p.parent for p in repo.glob("*/tokenizer.json"))
    if not snapshots:
        pytest.skip("Qwen3.5-2B tokenizer is not cached; this test never downloads")
    from transformers import AutoTokenizer

    return AutoTokenizer.from_pretrained(snapshots[0], local_files_only=True)


def test_text_template_preserves_official_format_and_masks_only_prompt():
    tokenizer = cached_tokenizer()
    from llamafactory.data import get_template_and_fix_tokenizer
    from llamafactory.data.processor.supervised import SupervisedDatasetProcessor
    from llamafactory.data.template import TEMPLATES
    from llamafactory.hparams import DataArguments

    train.register_text_template()
    args = DataArguments(template=train.TEXT_TEMPLATE, cutoff_len=128)
    template = get_template_and_fix_tokenizer(tokenizer, args)
    messages = [
        {"role": "user", "content": "PROMPT_ONLY_MARKER: Write a C function."},
        {"role": "assistant", "content": "int step(void) { return 1; }"},
    ]
    assert template.encode_oneturn(tokenizer, messages) == TEMPLATES[
        "qwen3_5_nothink"
    ].encode_oneturn(tokenizer, messages)
    processor = SupervisedDatasetProcessor(template, tokenizer, None, args)
    ids, labels = processor._encode_data_example(messages[:1], messages[1:], "", "", [], [], [])
    supervised = tokenizer.decode([x for x in labels if x != -100], skip_special_tokens=True)
    assert "PROMPT_ONLY_MARKER" not in supervised
    assert supervised.strip() == "int step(void) { return 1; }"
    assert all(x == -100 for x in labels[: labels.index(next(x for x in labels if x != -100))])
    assert len(ids) == len(labels)


def test_text_only_template_rejects_images():
    from llamafactory.data.template import TEMPLATES

    train.register_text_template()
    template = TEMPLATES[train.TEXT_TEMPLATE]
    with pytest.raises(ValueError, match="image input"):
        template.mm_plugin.process_messages([], ["unused.png"], [], [], None)
