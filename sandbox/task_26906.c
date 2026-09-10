#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// PID控制器结构体
typedef struct {
    float Kp;       // 比例系数
    float Ki;       // 积分系数
    float Kd;       // 微分系数
    float integral; // 积分项
    float prev_error; // 上一次误差
    float integral_limit; // 积分限幅
    float output_limit;   // 输出限幅
} PIDController;

// 电压环PID结构体
typedef struct {
    PIDController pid;
    float voltage_setpoint; // 电压设定值
    float voltage_feedback; // 电压反馈值
    float current_setpoint; // 输出电流设定值
} VoltagePID;

// 电流环PID结构体
typedef struct {
    PIDController pid;
    float current_setpoint; // 电流设定值
    float current_feedback; // 电流反馈值
    float duty_cycle;       // 输出占空比
} CurrentPID;

// PID初始化函数
void PID_Init(PIDController* pid, float Kp, float Ki, float Kd, float integral_limit, float output_limit) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
    pid->integral_limit = integral_limit;
    pid->output_limit = output_limit;
}

// PID计算函数
float PID_Calculate(PIDController* pid, float error, float dt) {
    // 比例项
    float proportional = pid->Kp * error;
    
    // 积分项
    pid->integral += error * dt;
    
    // 积分限幅
    if (pid->integral > pid->integral_limit) {
        pid->integral = pid->integral_limit;
    } else if (pid->integral < -pid->integral_limit) {
        pid->integral = -pid->integral_limit;
    }
    
    float integral_term = pid->Ki * pid->integral;
    
    // 微分项
    float derivative = (error - pid->prev_error) / dt;
    float derivative_term = pid->Kd * derivative;
    
    // 更新上一次误差
    pid->prev_error = error;
    
    // 计算输出
    float output = proportional + integral_term + derivative_term;
    
    // 输出限幅
    if (output > pid->output_limit) {
        output = pid->output_limit;
    } else if (output < -pid->output_limit) {
        output = -pid->output_limit;
    }
    
    return output;
}

// 电压环计算函数
void VoltagePID_Calculate(VoltagePID* vpid, float dt) {
    // 计算电压误差
    float voltage_error = vpid->voltage_setpoint - vpid->voltage_feedback;
    
    // 计算电流设定值
    vpid->current_setpoint = PID_Calculate(&vpid->pid, voltage_error, dt);
    
    // 电流设定值限幅（假设最大电流为10A）
    if (vpid->current_setpoint > 10.0f) {
        vpid->current_setpoint = 10.0f;
    } else if (vpid->current_setpoint < 0.0f) {
        vpid->current_setpoint = 0.0f;
    }
}

// 电流环计算函数
void CurrentPID_Calculate(CurrentPID* cpid, float dt) {
    // 计算电流误差
    float current_error = cpid->current_setpoint - cpid->current_feedback;
    
    // 计算占空比
    cpid->duty_cycle = PID_Calculate(&cpid->pid, current_error, dt);
    
    // 占空比限幅在0.0~1.0之间
    if (cpid->duty_cycle > 1.0f) {
        cpid->duty_cycle = 1.0f;
    } else if (cpid->duty_cycle < 0.0f) {
        cpid->duty_cycle = 0.0f;
    }
}

int main() {
    // 初始化电压环PID
    VoltagePID voltage_pid;
    PID_Init(&voltage_pid.pid, 0.5f, 0.1f, 0.01f, 5.0f, 5.0f); // 电压环参数
    voltage_pid.voltage_setpoint = 12.0f;
    
    // 初始化电流环PID
    CurrentPID current_pid;
    PID_Init(&current_pid.pid, 0.8f, 0.2f, 0.02f, 3.0f, 1.0f); // 电流环参数
    
    // 模拟控制周期
    float dt = 0.001f; // 控制周期1ms
    
    // 测试数据
    float voltage_feedback[] = {11.5f, 11.8f, 11.9f};
    float current_feedback[] = {3.0f, 3.2f, 3.1f};
    
    printf("串级PID控制器测试\n");
    printf("=================\n");
    
    for (int i = 0; i < 3; i++) {
        // 设置反馈值
        voltage_pid.voltage_feedback = voltage_feedback[i];
        current_pid.current_feedback = current_feedback[i];
        
        // 电压环计算
        VoltagePID_Calculate(&voltage_pid, dt);
        
        // 设置电流环的设定值为电压环的输出
        current_pid.current_setpoint = voltage_pid.current_setpoint;
        
        // 电流环计算
        CurrentPID_Calculate(&current_pid, dt);
        
        // 打印结果
        printf("周期 %d:\n", i + 1);
        printf("  电压设定值: %.2f V\n", voltage_pid.voltage_setpoint);
        printf("  电压反馈值: %.2f V\n", voltage_pid.voltage_feedback);
        printf("  电压误差: %.3f V\n", voltage_pid.voltage_setpoint - voltage_pid.voltage_feedback);
        printf("  电流设定值: %.3f A\n", current_pid.current_setpoint);
        printf("  电流反馈值: %.2f A\n", current_pid.current_feedback);
        printf("  电流误差: %.3f A\n", current_pid.current_setpoint - current_pid.current_feedback);
        printf("  最终占空比: %.3f\n", current_pid.duty_cycle);
        printf("\n");
    }
    
    return 0;
}