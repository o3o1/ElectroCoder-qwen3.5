@echo off
cd /d "%~dp0"
echo Compiling V/F control program...
gcc task_vf_control_fixed.c -lm -o task_vf_control_fixed.exe
if errorlevel 1 (
    echo Compilation failed
    exit /b 1
)
echo Running program...
task_vf_control_fixed.exe