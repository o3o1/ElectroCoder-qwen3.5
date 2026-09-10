#include <stdio.h>
#include <stdlib.h>

typedef struct {
    float Kp, Ki, Kd;
    float integral;
    float prev_error;
    float output_limit_min;
    float output_limit_max;
} PID_Controller;

void PID_Init(PID_Controller *pid, float Kp, float Ki, float Kd, float min, float max) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
    pid->output_limit_min = min;
    pid->output_limit_max = max;
}

float PID_Update(PID_Controller *pid, float setpoint, float measurement, float dt) {
    float error = setpoint - measurement;
    
    pid->integral += error * dt;
    
    float derivative = (error - pid->prev_error) / dt;
    
    float output = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * derivative;
    
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
    PID_Init(&pid, 1.0f, 0.1f, 0.05f, -10.0f, 10.0f);
    
    float setpoint = 50.0f;
    float measurements[] = {0.0f, 20.0f, 40.0f, 48.0f, 49.5f};
    int num_cycles = sizeof(measurements) / sizeof(measurements[0]);
    float dt = 0.1f;
    
    for (int i = 0; i < num_cycles; i++) {
        float measurement = measurements[i];
        float output = PID_Update(&pid, setpoint, measurement, dt);
        float error = setpoint - measurement;
        
        printf("Cycle %d: measurement=%.2f, error=%.2f, output=%.2f\n", 
               i+1, measurement, error, output);
    }
    
    return 0;
}