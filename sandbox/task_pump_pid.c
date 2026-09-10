#include <stdio.h>
#include <stdlib.h>

typedef struct {
    float Kp;
    float Ki;
    float Kd;
    float integral;
    float last_error;
} PIDController;

void pid_init(PIDController *pid, float Kp, float Ki, float Kd) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->integral = 0.0f;
    pid->last_error = 0.0f;
}

float pid_update(PIDController *pid, float setpoint, float measurement, float dt) {
    float error = setpoint - measurement;
    
    pid->integral += error * dt;
    
    float derivative = (error - pid->last_error) / dt;
    
    float output = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * derivative;
    
    pid->last_error = error;
    
    return output;
}

float limit_frequency(float freq) {
    if (freq < 0.0f) return 0.0f;
    if (freq > 50.0f) return 50.0f;
    return freq;
}

int main() {
    PIDController pid;
    pid_init(&pid, 2.0f, 0.5f, 0.1f);
    
    float setpoint = 1.0f;
    float measurements[] = {0.8f, 0.85f, 0.92f, 0.96f, 0.99f};
    int num_samples = 5;
    float dt = 0.1f;
    
    printf("水泵变频调速PID控制模拟\n");
    printf("目标压力: %.2f MPa\n", setpoint);
    printf("PID参数: Kp=%.1f, Ki=%.1f, Kd=%.1f\n", pid.Kp, pid.Ki, pid.Kd);
    printf("采样周期: %.1f秒\n\n", dt);
    printf("周期\t测量压力(MPa)\t输出频率(Hz)\n");
    printf("----------------------------------------\n");
    
    for (int i = 0; i < num_samples; i++) {
        float pid_output = pid_update(&pid, setpoint, measurements[i], dt);
        float frequency = pid_output + 25.0f;
        frequency = limit_frequency(frequency);
        
        printf("%d\t%.2f\t\t%.2f\n", i+1, measurements[i], frequency);
    }
    
    return 0;
}