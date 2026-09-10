@echo off
echo DC/DC变换器休眠唤醒控制模拟程序
echo ================================
echo.

rem 模拟硬件寄存器宏定义
set SLEEP_THRESHOLD=15
set WAKE_THRESHOLD=25

rem 状态定义
set STATE_ACTIVE=0
set STATE_SLEEP=1

rem 全局状态变量
set current_state=%STATE_ACTIVE%
set transition_count=0

rem 模拟寄存器
set load_current_reg=0
set control_reg=0

rem 模拟设置负载电流寄存器
:mock_set_load_current
set load_current_reg=%1
goto :eof

rem 模拟设置控制寄存器
:mock_set_control_reg
set control_reg=%1
goto :eof

rem DC/DC状态机函数
:dcdc_state_machine
call :mock_set_load_current %1

rem 状态转换逻辑
if "%current_state%"=="%STATE_ACTIVE%" (
    if %1 LSS %SLEEP_THRESHOLD% (
        set new_state=%STATE_SLEEP%
        call :mock_set_control_reg 1
    ) else (
        call :mock_set_control_reg 0
        set new_state=%current_state%
    )
) else (
    if %1 GTR %WAKE_THRESHOLD% (
        set new_state=%STATE_ACTIVE%
        call :mock_set_control_reg 2
    ) else (
        call :mock_set_control_reg 1
        set new_state=%current_state%
    )
)

rem 如果状态发生变化，记录转换
if not "%new_state%"=="%current_state%" (
    if "%current_state%"=="%STATE_ACTIVE%" (
        echo 状态: ACTIVE -> SLEEP, 电流: %1 mA
    ) else (
        echo 状态: SLEEP -> ACTIVE, 电流: %1 mA
    )
    set current_state=%new_state%
    set /a transition_count+=1
)
goto :eof

rem 状态名称转换函数
:get_state_name
if "%1"=="%STATE_ACTIVE%" (
    echo ACTIVE
) else (
    echo SLEEP
)
goto :eof

rem 主程序
echo 测试用例1: 负载变化序列 [30, 10, 40] mA
call :dcdc_state_machine 30
call :dcdc_state_machine 10
call :dcdc_state_machine 40

echo.
echo 测试用例2: 负载变化序列 [20, 14, 16, 26, 24] mA
call :dcdc_state_machine 20
call :dcdc_state_machine 14
call :dcdc_state_machine 16
call :dcdc_state_machine 26
call :dcdc_state_machine 24

echo.
echo 测试用例3: 负载变化序列 [5, 8, 12, 30, 35] mA
call :dcdc_state_machine 5
call :dcdc_state_machine 8
call :dcdc_state_machine 12
call :dcdc_state_machine 30
call :dcdc_state_machine 35

echo.
echo ================================
call :get_state_name %current_state% > temp.txt
set /p final_state=<temp.txt
del temp.txt
echo 最终状态: %final_state%
echo 总转换次数: %transition_count%

pause