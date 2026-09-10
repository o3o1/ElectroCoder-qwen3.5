#include <stdio.h>
#include <stdbool.h>

typedef struct {
    float Kp;
    float Ki;
    float Kd;
    float integral;
    float prev_error;
    float out_min;
    float out_max;
} PIDController;

void PID_Init(PIDController *pid, float Kp, float Ki, float Kd, float out_min, float out_max) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
    pid->out_min = out_min;
    pid->out_max = out_max;
}

float PID_Update(PIDController *pid, float setpoint, float measurement, float dt) {
    float error = setpoint - measurement;
    
    pid->integral += error * dt;
    
    float derivative = (error - pid->prev_error) / dt;
    
    float output = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * derivative;
    
    bool saturation = false;
    if (output > pid->out_max) {
        output = pid->out_max;
        saturation = true;
    } else if (output < pid->out_min) {
        output = pid->out_min;
        saturation = true;
    }
    
    if (saturation) {
        pid->integral -= error * dt;
    }
    
    pid->prev_error = error;
    
    return output;
}

int main() {
    PIDController pid;
    PID_Init(&pid, 0.5f, 0.1f, 0.01f, 0.0f, 1.0f);
    
    float setpoint = 5.0f;
    float measurement = 4.8f;
    float dt = 0.001f;
    
    for (int i = 0; i < 5; i++) {
        float output = PID_Update(&pid, setpoint, measurement, dt);
        printf("Cycle %d: measurement=%.2fV, PID_output=%.3f\n", i+1, measurement, output);
        measurement += 0.05f;
    }
    
    return 0;
}