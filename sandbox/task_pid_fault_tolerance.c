#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
    float Kp;
    float Ki;
    float Kd;
    float integral;
    float prev_error;
} PIDController;

float pid_compute(PIDController* pid, float setpoint, float measurement) {
    float error = setpoint - measurement;
    pid->integral += error;
    float derivative = error - pid->prev_error;
    pid->prev_error = error;
    
    float output = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * derivative;
    return output;
}

void pid_backup_params(PIDController* pid, PIDController* backup) {
    backup->Kp = pid->Kp;
    backup->Ki = pid->Ki;
    backup->Kd = pid->Kd;
    backup->integral = pid->integral;
    backup->prev_error = pid->prev_error;
}

void pid_restore_params(PIDController* pid, PIDController* backup) {
    pid->Kp = backup->Kp;
    pid->Ki = backup->Ki;
    pid->Kd = backup->Kd;
    pid->integral = backup->integral;
    pid->prev_error = backup->prev_error;
}

int pid_check_fault(PIDController* pid, float output) {
    if (output > 1000.0f || output < -1000.0f) {
        return 1;
    }
    return 0;
}

int main() {
    PIDController main_pid;
    PIDController backup_pid;
    
    main_pid.Kp = 1.5f;
    main_pid.Ki = 0.2f;
    main_pid.Kd = 0.1f;
    main_pid.integral = 0.0f;
    main_pid.prev_error = 0.0f;
    
    pid_backup_params(&main_pid, &backup_pid);
    
    float setpoint = 100.0f;
    float measurement = 0.0f;
    
    for (int i = 1; i <= 5; i++) {
        float output = pid_compute(&main_pid, setpoint, measurement);
        
        if (i == 3) {
            output = 1500.0f;
        }
        
        printf("Loop %d: output=%.2f, Kp=%.2f, Ki=%.2f, Kd=%.2f\n", 
               i, output, main_pid.Kp, main_pid.Ki, main_pid.Kd);
        
        if (pid_check_fault(&main_pid, output)) {
            printf("Fault detected! Restoring parameters...\n");
            printf("Before restore: Kp=%.2f, Ki=%.2f, Kd=%.2f\n", 
                   main_pid.Kp, main_pid.Ki, main_pid.Kd);
            
            pid_restore_params(&main_pid, &backup_pid);
            
            printf("After restore: Kp=%.2f, Ki=%.2f, Kd=%.2f\n", 
                   main_pid.Kp, main_pid.Ki, main_pid.Kd);
        }
        
        measurement += 20.0f;
    }
    
    return 0;
}