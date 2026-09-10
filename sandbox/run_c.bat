@echo off
cd /d "%~dp0"
echo 编译并运行滑动平均滤波器...
echo.

REM 检查是否有编译器
where gcc >nul 2>&1
if %errorlevel% equ 0 (
    echo 找到 gcc 编译器
    gcc -Wall -std=c99 task_sliding_average.c -o task_sliding_average.exe
    if %errorlevel% equ 0 (
        echo 编译成功
        echo.
        task_sliding_average.exe
    ) else (
        echo 编译失败
    )
    goto :end
)

where clang >nul 2>&1
if %errorlevel% equ 0 (
    echo 找到 clang 编译器
    clang -Wall -std=c99 task_sliding_average.c -o task_sliding_average.exe
    if %errorlevel% equ 0 (
        echo 编译成功
        echo.
        task_sliding_average.exe
    ) else (
        echo 编译失败
    )
    goto :end
)

where cc >nul 2>&1
if %errorlevel% equ 0 (
    echo 找到 cc 编译器
    cc -Wall -std=c99 task_sliding_average.c -o task_sliding_average.exe
    if %errorlevel% equ 0 (
        echo 编译成功
        echo.
        task_sliding_average.exe
    ) else (
        echo 编译失败
    )
    goto :end
)

echo 未找到C编译器
echo 显示源代码内容:
echo.
type task_sliding_average.c

:end
pause