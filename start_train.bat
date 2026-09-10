@echo off
chcp 65001 >nul
echo ========================================
echo  正在激活 Conda 环境: jzc
echo ========================================

:: 激活 Anaconda 环境，与你的安装路径匹配
call D:\Anaconda_envs\Scripts\activate.bat D:\Anaconda_envs\envs\jzc

:: 自动切换到当前脚本所在的项目目录
cd /d "%~dp0"

echo ========================================
echo  启动 LoRA 微调训练
echo ========================================

llamafactory-cli train ^
  --model_name_or_path "C:\Users\jiang\.lmstudio\models\lmstudio-community\Qwen2.5-Coder-7B-Instruct-GGUF\Qwen2.5-Coder-7B-Instruct-Q4_K_M.gguf" ^
  --dataset deepseek_ee_code ^
  --dataset_dir ./data ^
  --template qwen ^
  --stage sft ^
  --do_train ^
  --finetuning_type lora ^
  --per_device_train_batch_size 2 ^
  --gradient_accumulation_steps 4 ^
  --learning_rate 5e-5 ^
  --num_train_epochs 3 ^
  --lr_scheduler_type cosine ^
  --max_seq_length 2048 ^
  --output_dir ./output/qwen-ee-lora ^
  --logging_steps 10 ^
  --save_steps 200 ^
  --fp16

echo.
echo ========================================
echo  训练结束，按任意键退出
echo ========================================
pause >nul
