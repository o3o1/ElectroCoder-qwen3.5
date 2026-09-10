#include <stdio.h>

typedef struct {
    float kp;
    float ki;
    float kd;
    float integral;
    float prev_error;
    float output_limit_min;
    float output_limit_max;
} PID_Controller;

void PID_Init(PID_Controller *pid, float kp, float ki, float kd, float min, float max) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
    pid->output_limit_min = min;
    pid->output_limit_max = max;
}

float PID_Update(PID_Controller *pid, float setpoint, float measurement, float dt) {
    float error = setpoint - measurement;
    
    pid->integral += error * dt;
    
    float derivative = (error - pid->prev_error) / dt;
    
    float output = pid->kp * error + pid->ki * pid->integral + pid->kd * derivative;
    
    if (output < pid->output_limit_min) {
        output = pid->output_limit_min;
    } else if (output > pid->output_limit_max) {
        output = pid->output_limit_max;
    }
    
    pid->prev_error = error;
    
    return output;
}

int main() {
    PID_Controller pid;
    PID_Init(&pid, 1.0f, 0.5f, 0.1f, -10.0f, 10.0f);
    
    float setpoint = 100.0f;
    float measurement = 0.0f;
    float dt = 0.1f;
    
    printf("PID controller test (position type)\n");
    printf("Setpoint: %.2f, Initial measurement: %.2f, Time interval: %.2f seconds\n\n", setpoint, measurement, dt);
    
    for (int i = 0; i < 5; i++) {
        float output = PID_Update(&pid, setpoint, measurement, dt);
        
        printf("Iteration %d:\n", i + 1);
        printf("  Measurement: %.2f\n", measurement);
        printf("  Controller output: %.2f\n", output);
        
        measurement += output * 0.5f;
        
        printf("  Updated measurement: %.2f\n\n", measurement);
    }
    
    return 0;
}