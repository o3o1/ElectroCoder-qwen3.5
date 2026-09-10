@echo off
echo 油田抽油机变频调速节能控制算法模拟
echo ==================================================

setlocal enabledelayedexpansion

rem 定义计算频率的函数
:calculate_frequency
set "load_ratio=%~1"
set "freq=0"

rem 负载率 < 0.3: 节能模式，固定30Hz
if %load_ratio% LSS 0.3 (
    set "freq=30.00"
    goto :return_freq
)

rem 负载率 0.3~0.7: 线性调节，30Hz~55Hz
if %load_ratio% LEQ 0.7 (
    rem 计算: 30 + (load_ratio - 0.3) * 25
    set /a "temp=load_ratio * 100"
    set /a "temp2=temp - 30"
    set /a "temp3=temp2 * 25"
    set /a "temp4=3000 + temp3"
    set "freq=!temp4!"
    set "freq=!freq:~0,-2!.!freq:~-2!"
    goto :return_freq
)

rem 负载率 > 0.7: 额定模式，固定55Hz
set "freq=55.00"

:return_freq
echo %freq%
exit /b

rem 主程序
echo.
echo 测试不同负载情况:
echo.

set test_loads=0.15 0.45 0.85

for %%L in (%test_loads%) do (
    echo 测试负载率: %%L
    call :calculate_frequency %%L
    set "target_freq=!errorlevel!"
    echo 目标频率: !target_freq! Hz
    
    echo 调节PWM占空比...
    
    rem 计算目标PWM值 (假设PWM_MAX_VALUE=3000, MAX_FREQ=55)
    set /a "target_pwm=!target_freq! * 3000 / 55"
    
    rem 模拟调节过程
    set "current_pwm=1000"
    
    :adjust_loop
    if !current_pwm! LSS !target_pwm! (
        set /a "current_pwm+=100"
        if !current_pwm! GTR !target_pwm! set "current_pwm=!target_pwm!"
    ) else if !current_pwm! GTR !target_pwm! (
        set /a "current_pwm-=100"
        if !current_pwm! LSS !target_pwm! set "current_pwm=!target_pwm!"
    )
    
    echo 当前值: !current_pwm!
    
    if not !current_pwm!==!target_pwm! goto :adjust_loop
    
    echo 频率稳定在!target_freq! Hz
    echo.
)

echo ==================================================
echo 模拟完成!
echo.
echo 算法说明:
echo 1. 负载率 ^< 0.3: 节能模式，固定30Hz
echo 2. 负载率 0.3~0.7: 线性调节，30Hz~55Hz
echo 3. 负载率 ^> 0.7: 额定模式，固定55Hz

pause