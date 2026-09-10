@echo off
echo 正在编译Buck变换器PI控制程序...
if exist task_buck_pi_fixed.exe del task_buck_pi_fixed.exe
echo #include <stdio.h> > temp.c
echo #include <stdlib.h> >> temp.c
echo #include <string.h> >> temp.c
echo typedef struct { >> temp.c
echo     float kp; >> temp.c
echo     float ki; >> temp.c
echo     float integral; >> temp.c
echo     float integral_limit; >> temp.c
echo     int compensation_delay; >> temp.c
echo     float* compensation_buffer; >> temp.c
echo     int buffer_index; >> temp.c
echo } BuckPIController; >> temp.c
echo void buck_pi_init(BuckPIController* ctrl, float kp, float ki, float integral_limit, int delay_cycles) { >> temp.c
echo     ctrl->kp = kp; >> temp.c
echo     ctrl->ki = ki; >> temp.c
echo     ctrl->integral = 0.0f; >> temp.c
echo     ctrl->integral_limit = integral_limit; >> temp.c
echo     ctrl->compensation_delay = delay_cycles; >> temp.c
echo     if (delay_cycles > 0) { >> temp.c
echo         ctrl->compensation_buffer = (float*)malloc(delay_cycles * sizeof(float)); >> temp.c
echo         memset(ctrl->compensation_buffer, 0, delay_cycles * sizeof(float)); >> temp.c
echo     } else { >> temp.c
echo         ctrl->compensation_buffer = NULL; >> temp.c
echo     } >> temp.c
echo     ctrl->buffer_index = 0; >> temp.c
echo } >> temp.c
echo float buck_pi_update(BuckPIController* ctrl, float v_ref, float v_fb, float dt) { >> temp.c
echo     float error = v_ref - v_fb; >> temp.c
echo     ctrl->integral += ctrl->ki * error * dt; >> temp.c
echo     if (ctrl->integral > ctrl->integral_limit) { >> temp.c
echo         ctrl->integral = ctrl->integral_limit; >> temp.c
echo     } else if (ctrl->integral < -ctrl->integral_limit) { >> temp.c
echo         ctrl->integral = -ctrl->integral_limit; >> temp.c
echo     } >> temp.c
echo     float proportional = ctrl->kp * error; >> temp.c
echo     float output = proportional + ctrl->integral; >> temp.c
echo     if (ctrl->compensation_delay > 0) { >> temp.c
echo         ctrl->compensation_buffer[ctrl->buffer_index] = output; >> temp.c
echo         ctrl->buffer_index = (ctrl->buffer_index + 1) %% ctrl->compensation_delay; >> temp.c
echo         int delayed_index = (ctrl->buffer_index + ctrl->compensation_delay - 1) %% ctrl->compensation_delay; >> temp.c
echo         return ctrl->compensation_buffer[delayed_index]; >> temp.c
echo     } >> temp.c
echo     return output; >> temp.c
echo } >> temp.c
echo void buck_pi_cleanup(BuckPIController* ctrl) { >> temp.c
echo     if (ctrl->compensation_buffer != NULL) { >> temp.c
echo         free(ctrl->compensation_buffer); >> temp.c
echo         ctrl->compensation_buffer = NULL; >> temp.c
echo     } >> temp.c
echo } >> temp.c
echo int main() { >> temp.c
echo     BuckPIController ctrl; >> temp.c
echo     buck_pi_init(&ctrl, 0.5f, 10.0f, 1.0f, 3); >> temp.c
echo     float dt = 0.001f; >> temp.c
echo     float v_ref_values[] = {5.0f, 5.0f, 5.0f, 5.0f, 5.0f}; >> temp.c
echo     float v_fb_values[] = {4.5f, 4.8f, 4.9f, 5.0f, 5.1f}; >> temp.c
echo     printf("Buck变换器电压闭环PI控制仿真\\n"); >> temp.c
echo     printf("参数: kp=%.1f, ki=%.1f, 积分限幅=%.1f, 延迟补偿=%%d周期\\n\\n", >> temp.c
echo            ctrl.kp, ctrl.ki, ctrl.integral_limit, ctrl.compensation_delay); >> temp.c
echo     printf("周期\\tv_ref\\tv_fb\\t误差\\t\\t积分值\\t\\t补偿前输出\\t补偿后输出\\n"); >> temp.c
echo     printf("-----------------------------------------------------------------------------\\n"); >> temp.c
echo     for (int i = 0; i < 5; i++) { >> temp.c
echo         float v_ref = v_ref_values[i]; >> temp.c
echo         float v_fb = v_fb_values[i]; >> temp.c
echo         float error = v_ref - v_fb; >> temp.c
echo         float integral_before = ctrl.integral; >> temp.c
echo         float output_before = ctrl.kp * error + integral_before; >> temp.c
echo         float output_compensated = buck_pi_update(&ctrl, v_ref, v_fb, dt); >> temp.c
echo         printf("%d\\t%.1f\\t%.1f\\t%.3f\\t\\t%.6f\\t%.6f\\t%.6f\\n", >> temp.c
echo                i+1, v_ref, v_fb, error, ctrl.integral, output_before, output_compensated); >> temp.c
echo     } >> temp.c
echo     buck_pi_cleanup(&ctrl); >> temp.c
echo     return 0; >> temp.c
echo } >> temp.c

echo 正在尝试编译...
where gcc >nul 2>&1
if %errorlevel% equ 0 (
    echo 使用gcc编译...
    gcc -Wall -std=c99 temp.c -o task_buck_pi_fixed.exe
    if %errorlevel% equ 0 (
        echo 编译成功！
        echo.
        task_buck_pi_fixed.exe
    ) else (
        echo gcc编译失败
    )
    goto :end
)

where cl >nul 2>&1
if %errorlevel% equ 0 (
    echo 使用cl编译...
    cl temp.c /Fe:task_buck_pi_fixed.exe
    if %errorlevel% equ 0 (
        echo 编译成功！
        echo.
        task_buck_pi_fixed.exe
    ) else (
        echo cl编译失败
    )
    goto :end
)

where cc >nul 2>&1
if %errorlevel% equ 0 (
    echo 使用cc编译...
    cc temp.c -o task_buck_pi_fixed.exe
    if %errorlevel% equ 0 (
        echo 编译成功！
        echo.
        task_buck_pi_fixed.exe
    ) else (
        echo cc编译失败
    )
    goto :end
)

echo 未找到C编译器！
echo 请安装gcc、cl或cc编译器

:end
if exist temp.c del temp.c
pause