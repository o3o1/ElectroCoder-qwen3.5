@echo off
echo 编译C程序...
gcc -Wall -lm task_inverter_harmonic.c -o task_inverter_harmonic.exe
if %errorlevel% neq 0 (
    echo 编译失败
    exit /b 1
)
echo 运行程序...
task_inverter_harmonic.exe