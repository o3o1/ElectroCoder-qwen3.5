@echo off
echo 编译单相锁相环程序...
gcc -Wall -lm task_pll.c -o task_pll.exe
if %errorlevel% neq 0 (
    echo 编译失败
    pause
    exit /b 1
)
echo 编译成功，运行程序...
echo.
task_pll.exe
echo.
pause