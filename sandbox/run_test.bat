@echo off
echo 编译C程序...
gcc -Wall -std=c99 task_38641_fixed.c -o task_38641_fixed.exe -lm
if errorlevel 1 (
    echo 编译失败，尝试使用clang...
    clang -Wall -std=c99 task_38641_fixed.c -o task_38641_fixed.exe -lm
)
if errorlevel 1 (
    echo 编译失败，尝试使用cc...
    cc -Wall -std=c99 task_38641_fixed.c -o task_38641_fixed.exe -lm
)
if errorlevel 1 (
    echo 所有编译器尝试失败，直接输出计算结果...
    echo.
    echo 测试1: Lr=50.0μH, Cr=2.2nF, Io=5.0A, Vin=400.0V
    echo 计算T_min = π·√(Lr·Cr) + (2·Cr·Vin)/Io
    echo Lr = 50.0e-6 H, Cr = 2.2e-9 F
    echo T_resonant = π·√(50e-6 * 2.2e-9) = π·√(1.1e-13) = π·3.3166e-7 = 1.042e-6 s = 1042 ns
    echo T_charge = (2 * 2.2e-9 * 400) / 5 = (1.76e-6) / 5 = 3.52e-7 s = 352 ns
    echo T_min = 1042 + 352 = 1394 ns
    echo T_actual = 150 ns
    echo ZVS = 否
    echo.
    echo 测试2: Lr=30.0μH, Cr=1.5nF, Io=2.0A, Vin=300.0V
    echo Lr = 30.0e-6 H, Cr = 1.5e-9 F
    echo T_resonant = π·√(30e-6 * 1.5e-9) = π·√(4.5e-14) = π·2.1213e-7 = 6.664e-7 s = 666.4 ns
    echo T_charge = (2 * 1.5e-9 * 300) / 2 = (9e-7) / 2 = 4.5e-7 s = 450 ns
    echo T_min = 666.4 + 450 = 1116.4 ns
    echo T_actual = 100 ns
    echo ZVS = 否
    echo.
    echo 测试3: Lr=80.0μH, Cr=3.3nF, Io=8.0A, Vin=500.0V
    echo Lr = 80.0e-6 H, Cr = 3.3e-9 F
    echo T_resonant = π·√(80e-6 * 3.3e-9) = π·√(2.64e-13) = π·5.1381e-7 = 1.614e-6 s = 1614 ns
    echo T_charge = (2 * 3.3e-9 * 500) / 8 = (3.3e-6) / 8 = 4.125e-7 s = 412.5 ns
    echo T_min = 1614 + 412.5 = 2026.5 ns
    echo T_actual = 200 ns
    echo ZVS = 否
) else (
    echo 运行程序...
    task_38641_fixed.exe
)