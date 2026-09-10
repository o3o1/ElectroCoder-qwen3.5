@echo off
echo Compiling task_power_decoupling_fixed.c...
gcc -Wall -std=c99 task_power_decoupling_fixed.c -o task_power_decoupling_fixed.exe
if %errorlevel% neq 0 (
    echo Compilation failed!
    exit /b 1
)
echo Running the program...
task_power_decoupling_fixed.exe