#include <stdio.h>
#include <stdlib.h>

typedef struct {
    double Kp, Ki, Kd;
    double integral;
    double prev_error;
    int stage;
} PIDController;

void update_pid_params(PIDController *pid, int stage) {
    pid->stage = stage;
    switch(stage) {
        case 0:
            pid->Kp = 0.5;
            pid->Ki = 0.1;
            pid->Kd = 0.05;
            break;
        case 1:
            pid->Kp = 0.3;
            pid->Ki = 0.05;
            pid->Kd = 0.02;
            break;
        case 2:
            pid->Kp = 0.8;
            pid->Ki = 0.2;
            pid->Kd = 0.1;
            break;
        default:
            break;
    }
}

double pid_compute(PIDController *pid, double setpoint, double measurement, double dt) {
    double error = setpoint - measurement;
    pid->integral += error * dt;
    double derivative = (error - pid->prev_error) / dt;
    double output = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * derivative;
    pid->prev_error = error;
    return output;
}

int main() {
    PIDController pid;
    pid.integral = 0.0;
    pid.prev_error = 0.0;
    
    update_pid_params(&pid, 0);
    
    printf("Stage 0: Startup Phase\n");
    double measurements_0[] = {4.0, 4.4, 4.8};
    for(int i = 0; i < 3; i++) {
        double output = pid_compute(&pid, 5.0, measurements_0[i], 0.01);
        printf("  Stage: %d, Setpoint: %.2f, Measurement: %.2f, PID Output: %.4f\n", 
               pid.stage, 5.0, measurements_0[i], output);
    }
    
    update_pid_params(&pid, 1);
    printf("\nStage 1: Steady State Phase\n");
    double measurements_1[] = {4.9, 5.0, 5.1};
    for(int i = 0; i < 3; i++) {
        double output = pid_compute(&pid, 5.0, measurements_1[i], 0.01);
        printf("  Stage: %d, Setpoint: %.2f, Measurement: %.2f, PID Output: %.4f\n", 
               pid.stage, 5.0, measurements_1[i], output);
    }
    
    update_pid_params(&pid, 2);
    printf("\nStage 2: Dynamic Response Phase\n");
    double measurements_2[] = {5.0, 4.5, 4.8};
    for(int i = 0; i < 3; i++) {
        double output = pid_compute(&pid, 5.0, measurements_2[i], 0.01);
        printf("  Stage: %d, Setpoint: %.2f, Measurement: %.2f, PID Output: %.4f\n", 
               pid.stage, 5.0, measurements_2[i], output);
    }
    
    return 0;
}