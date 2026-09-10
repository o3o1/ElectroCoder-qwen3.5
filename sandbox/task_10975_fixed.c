#include <stdio.h>
#include <math.h>

typedef struct {
    float Kp, Ki, Kd;
    float Ts;
    float integral;
    float prev_error;
    float prev_output;
} DiscretePID;

void pid_init(DiscretePID *pid, float Kp, float Ki, float Kd, float Ts) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->Ts = Ts;
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
    pid->prev_output = 0.0f;
}

float pid_update(DiscretePID *pid, float setpoint, float measurement) {
    float error = setpoint - measurement;
    
    // 比例项
    float proportional = pid->Kp * error;
    
    // 积分项 - Tustin变换（梯形积分）
    float integral_term = pid->Ki * pid->Ts * (error + pid->prev_error) / 2.0f;
    
    // 微分项 - Tustin变换（一阶后向差分）
    float derivative_term = pid->Kd * (error - pid->prev_error) / pid->Ts;
    
    // 计算输出
    float output = proportional + pid->integral + derivative_term;
    
    // 保存当前误差用于下一次计算
    pid->prev_error = error;
    
    // 抗积分饱和处理
    if (output > 10.0f) {
        output = 10.0f;
    } else if (output < -10.0f) {
        output = -10.0f;
    } else {
        // 只有输出未饱和时才累积积分
        pid->integral += integral_term;
    }
    
    pid->prev_output = output;
    return output;
}

int main() {
    printf("离散化PID控制器测试（Tustin变换）\n");
    printf("===================================\n\n");
    
    // 测试1：阶跃响应测试
    printf("测试1：阶跃响应测试（设定值从0突变为5）\n");
    printf("周期\t设定值\t测量值\t输出\n");
    DiscretePID pid1;
    pid_init(&pid1, 1.0f, 0.5f, 0.1f, 0.1f);
    
    float measurement1 = 0.0f;
    for (int i = 0; i < 10; i++) {
        float setpoint1 = (i == 0) ? 0.0f : 5.0f;
        float output1 = pid_update(&pid1, setpoint1, measurement1);
        printf("%d\t%.2f\t%.2f\t%.2f\n", i, setpoint1, measurement1, output1);
        measurement1 += output1 * 0.1f;
    }
    printf("\n");
    
    // 测试2：斜坡跟踪测试
    printf("测试2：斜坡跟踪测试（设定值以0.5每周期递增）\n");
    printf("周期\t设定值\t测量值\t输出\n");
    DiscretePID pid2;
    pid_init(&pid2, 1.0f, 0.3f, 0.05f, 0.1f);
    
    float measurement2 = 0.0f;
    for (int i = 0; i < 8; i++) {
        float setpoint2 = i * 0.5f;
        float output2 = pid_update(&pid2, setpoint2, measurement2);
        printf("%d\t%.2f\t%.2f\t%.2f\n", i, setpoint2, measurement2, output2);
        measurement2 += output2 * 0.1f;
    }
    printf("\n");
    
    // 测试3：抗饱和测试
    printf("测试3：抗饱和测试（设定值=100，输出限制在[-10,10]）\n");
    printf("周期\t设定值\t测量值\t输出\n");
    DiscretePID pid3;
    pid_init(&pid3, 1.0f, 0.5f, 0.1f, 0.1f);
    
    float measurement3 = 0.0f;
    for (int i = 0; i < 5; i++) {
        float setpoint3 = 100.0f;
        float output3 = pid_update(&pid3, setpoint3, measurement3);
        printf("%d\t%.2f\t%.2f\t%.2f\n", i, setpoint3, measurement3, output3);
        measurement3 += output3 * 0.1f;
    }
    
    return 0;
}