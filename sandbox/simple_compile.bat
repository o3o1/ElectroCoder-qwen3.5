@echo off
echo 正在编译Buck变换器PI控制程序...
echo 检查编译器...

where gcc
if %errorlevel% equ 0 (
    echo 找到gcc编译器
    gcc -Wall -std=c99 task_buck_pi_fixed.c -o task_buck_pi_fixed.exe
    if %errorlevel% equ 0 (
        echo 编译成功！
        echo.
        task_buck_pi_fixed.exe
    ) else (
        echo gcc编译失败
    )
    goto :end
)

where cl
if %errorlevel% equ 0 (
    echo 找到cl编译器
    cl task_buck_pi_fixed.c /Fe:task_buck_pi_fixed.exe
    if %errorlevel% equ 0 (
        echo 编译成功！
        echo.
        task_buck_pi_fixed.exe
    ) else (
        echo cl编译失败
    )
    goto :end
)

echo 未找到C编译器！
echo 请安装gcc或Visual Studio编译器

:end
pause