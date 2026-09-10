#include <stdio.h>

typedef struct {
    float Kp, Ki, Kd;
    float integral;
    float prev_error;
    float integral_limit;
    float output_limit;
} PIDController;

typedef struct {
    PIDController pid;
    float voltage_setpoint;
    float voltage_feedback;
    float current_setpoint;
} VoltagePID;

typedef struct {
    PIDController pid;
    float current_setpoint;
    float current_feedback;
    float duty_cycle;
} CurrentPID;

void PID_Init(PIDController* pid, float Kp, float Ki, float Kd, float integral_limit, float output_limit) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
    pid->integral_limit = integral_limit;
    pid->output_limit = output_limit;
}

float PID_Calculate(PIDController* pid, float error, float dt) {
    float proportional = pid->Kp * error;
    
    pid->integral += error * dt;
    if (pid->integral > pid->integral_limit) pid->integral = pid->integral_limit;
    if (pid->integral < -pid->integral_limit) pid->integral = -pid->integral_limit;
    
    float integral_term = pid->Ki * pid->integral;
    float derivative = (error - pid->prev_error) / dt;
    float derivative_term = pid->Kd * derivative;
    
    pid->prev_error = error;
    
    float output = proportional + integral_term + derivative_term;
    
    if (output > pid->output_limit) output = pid->output_limit;
    if (output < -pid->output_limit) output = -pid->output_limit;
    
    return output;
}

void VoltagePID_Calculate(VoltagePID* vpid, float dt) {
    float voltage_error = vpid->voltage_setpoint - vpid->voltage_feedback;
    vpid->current_setpoint = PID_Calculate(&vpid->pid, voltage_error, dt);
    
    if (vpid->current_setpoint > 10.0f) vpid->current_setpoint = 10.0f;
    if (vpid->current_setpoint < 0.0f) vpid->current_setpoint = 0.0f;
}

void CurrentPID_Calculate(CurrentPID* cpid, float dt) {
    float current_error = cpid->current_setpoint - cpid->current_feedback;
    cpid->duty_cycle = PID_Calculate(&cpid->pid, current_error, dt);
    
    if (cpid->duty_cycle > 1.0f) cpid->duty_cycle = 1.0f;
    if (cpid->duty_cycle < 0.0f) cpid->duty_cycle = 0.0f;
}

int main() {
    VoltagePID voltage_pid;
    PID_Init(&voltage_pid.pid, 0.5f, 0.1f, 0.01f, 5.0f, 5.0f);
    voltage_pid.voltage_setpoint = 12.0f;
    
    CurrentPID current_pid;
    PID_Init(&current_pid.pid, 0.8f, 0.2f, 0.02f, 3.0f, 1.0f);
    
    float dt = 0.001f;
    
    float voltage_feedback[] = {11.5f, 11.8f, 11.9f};
    float current_feedback[] = {3.0f, 3.2f, 3.1f};
    
    printf("Cascade PID Test\n");
    printf("================\n");
    
    for (int i = 0; i < 3; i++) {
        voltage_pid.voltage_feedback = voltage_feedback[i];
        current_pid.current_feedback = current_feedback[i];
        
        VoltagePID_Calculate(&voltage_pid, dt);
        current_pid.current_setpoint = voltage_pid.current_setpoint;
        CurrentPID_Calculate(&current_pid, dt);
        
        printf("Cycle %d:\n", i + 1);
        printf("  Voltage error: %.3f V\n", voltage_pid.voltage_setpoint - voltage_pid.voltage_feedback);
        printf("  Current error: %.3f A\n", current_pid.current_setpoint - current_pid.current_feedback);
        printf("  Duty cycle: %.3f\n", current_pid.duty_cycle);
        printf("\n");
    }
    
    return 0;
}