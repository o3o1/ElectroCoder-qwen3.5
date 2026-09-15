#!/usr/bin/env bash
# All GPU work is sequential. Model weights must already be cached locally.
set -euo pipefail
cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.."

test_model="${1:-}"
if [[ -z "$test_model" ]]; then
  test_model="$(uv run --locked python - <<'PY'
import json
from pathlib import Path
from huggingface_hub import snapshot_download
from huggingface_hub.errors import LocalEntryNotFoundError

for repo in ("Qwen/Qwen3.5-2B", "Qwen/Qwen3.5-0.8B"):
    try:
        path = Path(snapshot_download(repo, local_files_only=True))
    except LocalEntryNotFoundError:
        continue
    index = path / "model.safetensors.index.json"
    names = (set(json.loads(index.read_text())["weight_map"].values())
             if index.is_file() else {"model.safetensors"})
    if all((path / name).is_file() for name in names | {"config.json", "tokenizer.json"}):
        print(path)
        break
else:
    raise SystemExit("No complete cached Qwen3.5 2B/0.8B model; pass an existing small model directory.")
PY
)"
fi

test_root="output/local-validation-$(date -u +%Y%m%d-%H%M%S)"
common=(--model "$test_model" --offline --batch-size 1
        --gradient-accumulation-steps 1 --cutoff-len 1024 --precision bf16)
printf 'Cached test model: %s\nValidation output: %s\n' "$test_model" "$test_root"
for quantization in none nf4 fp4; do
  run_dir="$test_root/$quantization"
  uv run --locked train.py "${common[@]}" --quantization "$quantization" \
    --smoke-test --output-dir "$run_dir"
  uv run --locked verify_adapter.py --run-dir "$run_dir"
  # Resume the intermediate checkpoint with the original total step count/scheduler.
  uv run --locked train.py "${common[@]}" --quantization "$quantization" \
    --max-steps 2 --save-steps 1 --resume-from-checkpoint "$run_dir/checkpoint-1"
done
printf 'All three GPU modes passed: %s\n' "$test_root"
