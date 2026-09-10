#include <stdio.h>

typedef struct {
    float kp;
    float ki;
    float kd;
    float integral;
    float prev_error;
    float output_min;
    float output_max;
} pid_controller_t;

void pid_init(pid_controller_t *pid, float kp, float ki, float kd, float min, float max) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
    pid->output_min = min;
    pid->output_max = max;
}

float pid_update(pid_controller_t *pid, float setpoint, float measurement, float dt) {
    float error = setpoint - measurement;
    pid->integral += error * dt;
    float derivative = (error - pid->prev_error) / dt;
    
    float output = pid->kp * error + pid->ki * pid->integral + pid->kd * derivative;
    
    if (output < pid->output_min) {
        output = pid->output_min;
    } else if (output > pid->output_max) {
        output = pid->output_max;
    }
    
    pid->prev_error = error;
    return output;
}

int main() {
    pid_controller_t pid;
    pid_init(&pid, 1.0f, 0.1f, 0.05f, -10.0f, 10.0f);
    
    float setpoint = 5.0f;
    float measurements[] = {0.0f, 2.0f, 4.0f, 4.5f, 4.8f};
    float dt = 0.1f;
    
    for (int i = 0; i < 5; i++) {
        float output = pid_update(&pid, setpoint, measurements[i], dt);
        printf("周期 %d: 测量值=%.2f, PID输出=%.2f\n", i+1, measurements[i], output);
    }
    
    return 0;
}