@echo off
echo Compiling Boost PID simulation...
gcc -Wall -lm task_boost_pid.c -o task_boost_pid.exe
if %errorlevel% neq 0 (
    echo Compilation failed
    exit /b 1
)
echo Running simulation...
task_boost_pid.exe