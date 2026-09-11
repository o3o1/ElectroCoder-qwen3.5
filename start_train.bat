@echo off
chcp 65001 >nul
echo ========================================
echo  启动 LoRA 微调训练
echo ========================================

cd /d "%~dp0"

"D:\Anaconda_envs\envs\jzc\Scripts\llamafactory-cli.exe" train ^
  --model_name_or_path "D:/LLM_models/models/qwen--Qwen2.5-Coder-7B-Instruct/snapshots/master" ^
  --dataset deepseek_ee_code ^
  --dataset_dir ./data ^
  --template qwen ^
  --stage sft ^
  --do_train ^
  --finetuning_type lora ^
  --quantization_bit 4 ^
  --upcast_layernorm true ^
  --per_device_train_batch_size 2 ^
  --gradient_accumulation_steps 4 ^
  --learning_rate 5e-5 ^
  --num_train_epochs 3 ^
  --lr_scheduler_type cosine ^
  --cutoff_len 2048 ^
  --output_dir ./output/qwen-ee-lora ^
  --logging_steps 10 ^
  --save_steps 200 ^
  --fp16

echo.
echo ========================================
echo  训练结束，按任意键退出
echo ========================================
pause >nul
