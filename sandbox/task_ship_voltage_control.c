#include <stdio.h>
#include <stdlib.h>

typedef struct {
    float Kp;
    float Ki;
    float Kd;
    float integral;
    float prev_error;
} PID_Controller;

void PID_Init(PID_Controller *pid, float kp, float ki, float kd) {
    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
}

float PID_Update(PID_Controller *pid, float setpoint, float measured, float dt) {
    float error = setpoint - measured;
    
    pid->integral += error * dt;
    
    float derivative = (error - pid->prev_error) / dt;
    
    float output = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * derivative;
    
    pid->prev_error = error;
    
    return output;
}

int main() {
    PID_Controller pid;
    PID_Init(&pid, 0.8f, 0.2f, 0.1f);
    
    float setpoint = 400.0f;
    float measured_voltages[] = {390.0f, 395.0f, 398.0f, 401.0f, 400.0f};
    float dt = 0.1f;
    int steps = 5;
    
    printf("船舶电力系统PID稳压控制模拟\n");
    printf("设定电压: %.1f V\n", setpoint);
    printf("PID参数: Kp=%.1f, Ki=%.1f, Kd=%.1f\n", pid.Kp, pid.Ki, pid.Kd);
    printf("时间步长: %.1f 秒\n\n", dt);
    
    for (int i = 0; i < steps; i++) {
        float measured = measured_voltages[i];
        float output = PID_Update(&pid, setpoint, measured, dt);
        
        printf("时间步 %d: 测量电压=%.1f V, PID输出调整量=%.3f\n", 
               i+1, measured, output);
    }
    
    printf("\n积分项最终值: %.3f\n", pid.integral);
    
    return 0;
}