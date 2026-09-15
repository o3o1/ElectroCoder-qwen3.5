import copy
import json
from pathlib import Path

import pytest

import train


def dataset(tmp_path, rows=None):
    folder = tmp_path / "data"
    folder.mkdir()
    rows = (
        rows
        if rows is not None
        else [
            {"instruction": "Write a PI controller", "input": "C", "output": "float step() {}"},
            {"instruction": "Explain PWM", "input": "", "output": "Pulse width modulation"},
        ]
    )
    train.dump_json(
        folder / "dataset_info.json",
        {"deepseek_ee_code": {"file_name": "train.jsonl", "formatting": "alpaca"}},
    )
    (folder / "train.jsonl").write_text("\n".join(json.dumps(r) for r in rows))
    return folder


def test_yaml_cli_precedence_and_paths(tmp_path):
    config_file = tmp_path / "settings.yaml"
    config_file.write_text("batch_size: 2\nprecision: fp16\ndataset_dir: data\n")
    config, _ = train.resolve_config(
        [
            "--config",
            str(config_file),
            "--batch-size",
            "3",
            "--precision",
            "bf16",
            "--no-double-quantization",
        ]
    )
    assert config["batch_size"] == 3
    assert config["dataset_dir"] == str(tmp_path / "data")
    assert config["precision"] == "bf16"
    assert config["double_quantization"] is False
    assert config["lora_rank"] == 8


@pytest.mark.parametrize("mode", ["none", "nf4", "fp4"])
@pytest.mark.parametrize("precision", ["fp32", "bf16", "fp16"])
def test_precision_and_quantization_independent(mode, precision):
    config, _ = train.resolve_config(["--quantization", mode, "--precision", precision])
    native = train.factory_config(config)
    assert native["quantization_bit"] == (None if mode == "none" else 4)
    assert native["bf16"] == (precision == "bf16")
    assert native["fp16"] == (precision == "fp16")
    assert native["quantization_type"] == ("nf4" if mode == "none" else mode)


@pytest.mark.parametrize(
    "key,value",
    [
        ("batch_size", 0),
        ("batch_size", 1.5),
        ("batch_size", True),
        ("learning_rate", float("nan")),
        ("precision", "fp4"),
        ("val_size", 1),
        ("warmup_ratio", -0.1),
        ("double_quantization", "false"),
        ("epochs", 0),
        ("max_steps", 0),
        ("gpu", -1),
        ("lora_rank", -8),
    ],
)
def test_invalid_configuration_rejected(key, value):
    config = train.DEFAULTS.copy()
    config[key] = value
    with pytest.raises(ValueError):
        train.validate_config(config)


def test_unknown_yaml_option_rejected(tmp_path):
    config = tmp_path / "bad.yaml"
    config.write_text("lerning_rate: 0.1\n")
    with pytest.raises(ValueError, match="Unknown YAML"):
        train.resolve_config(["--config", str(config)])


def test_smoke_does_not_change_default_config():
    original = copy.deepcopy(train.DEFAULTS)
    config, mode = train.resolve_config(["--smoke-test", "--cutoff-len", "1024"])
    assert mode == "smoke_test"
    assert config["max_steps"] == 2 and config["save_steps"] == 1
    assert config["cutoff_len"] == 1024
    assert train.DEFAULTS == original


def test_warmup_ratio_uses_transformers_v5_interface():
    config, _ = train.resolve_config(["--warmup-ratio", "0.1"])
    native = train.factory_config(config)
    assert native["warmup_steps"] == 0.1
    assert "warmup_ratio" not in native
    assert "save_safetensors" not in native


def test_dry_run_without_gpu_or_output(tmp_path, capsys):
    folder = dataset(tmp_path)
    output = tmp_path / "output"
    assert train.main(["--dry-run", "--dataset-dir", str(folder), "--output-dir", str(output)]) == 0
    assert not output.exists()
    result = json.loads(capsys.readouterr().out)
    assert result["data"]["samples"] == 2


def test_invalid_answer_rejected(tmp_path):
    folder = dataset(tmp_path, [{"instruction": "question", "input": "", "output": "  "}])
    with pytest.raises(ValueError, match="line 1"):
        train.read_data({**train.DEFAULTS, "dataset_dir": str(folder)})


def test_output_requires_explicit_resume(tmp_path):
    output = tmp_path / "run"
    output.mkdir()
    (output / "important.txt").write_text("keep")
    config, _ = train.resolve_config(["--output-dir", str(output)])
    with pytest.raises(ValueError, match="new/empty"):
        train.validate_output(config, {})
    assert (output / "important.txt").read_text() == "keep"


def checkpoint_fixture(tmp_path):
    folder = dataset(tmp_path)
    output = tmp_path / "run"
    checkpoint = output / "checkpoint-2"
    checkpoint.mkdir(parents=True)
    config, _ = train.resolve_config(
        [
            "--dataset-dir",
            str(folder),
            "--output-dir",
            str(output),
            "--max-steps",
            "2",
        ]
    )
    _, info = train.read_data(config)
    train.dump_json(output / "effective_config.json", {"options": config})
    train.dump_json(output / "data_report.json", info)
    train.dump_json(checkpoint / "trainer_state.json", {"global_step": 2})
    for name in (
        "optimizer.pt",
        "scheduler.pt",
        "adapter_config.json",
        "adapter_model.safetensors",
    ):
        (checkpoint / name).touch()
    resumed, _ = train.resolve_config(
        [
            "--dataset-dir",
            str(folder),
            "--resume-from-checkpoint",
            str(checkpoint),
            "--max-steps",
            "3",
        ]
    )
    return resumed, info


def test_resume_infers_output_and_accepts_larger_total_steps(tmp_path):
    config, info = checkpoint_fixture(tmp_path)
    assert config["output_dir"] == str(tmp_path / "run")
    train.validate_output(config, info)


def test_resume_rejects_changed_precision_and_dataset(tmp_path):
    config, info = checkpoint_fixture(tmp_path)
    with pytest.raises(ValueError, match="precision"):
        train.validate_output({**config, "precision": "fp16"}, info)
    with pytest.raises(ValueError, match="Dataset changed"):
        train.validate_output(config, {**info, "sha256": {"train.jsonl": "changed"}})


def test_resume_rejects_adapter_only_directory(tmp_path):
    config, info = checkpoint_fixture(tmp_path)
    (Path(config["resume_from_checkpoint"]) / "optimizer.pt").unlink()
    with pytest.raises(ValueError, match="optimizer.pt"):
        train.validate_output(config, info)


def test_resume_requires_more_steps(tmp_path):
    config, info = checkpoint_fixture(tmp_path)
    config["max_steps"] = 2
    with pytest.raises(ValueError, match="global_step"):
        train.validate_output(config, info)
