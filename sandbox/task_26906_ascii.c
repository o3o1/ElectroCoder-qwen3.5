#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// PID controller structure
typedef struct {
    float Kp;       // Proportional coefficient
    float Ki;       // Integral coefficient
    float Kd;       // Derivative coefficient
    float integral; // Integral term
    float prev_error; // Previous error
    float integral_limit; // Integral limit
    float output_limit;   // Output limit
} PIDController;

// Voltage loop PID structure
typedef struct {
    PIDController pid;
    float voltage_setpoint; // Voltage setpoint
    float voltage_feedback; // Voltage feedback
    float current_setpoint; // Output current setpoint
} VoltagePID;

// Current loop PID structure
typedef struct {
    PIDController pid;
    float current_setpoint; // Current setpoint
    float current_feedback; // Current feedback
    float duty_cycle;       // Output duty cycle
} CurrentPID;

// PID initialization function
void PID_Init(PIDController* pid, float Kp, float Ki, float Kd, float integral_limit, float output_limit) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
    pid->integral_limit = integral_limit;
    pid->output_limit = output_limit;
}

// PID calculation function
float PID_Calculate(PIDController* pid, float error, float dt) {
    // Proportional term
    float proportional = pid->Kp * error;
    
    // Integral term
    pid->integral += error * dt;
    
    // Integral limit
    if (pid->integral > pid->integral_limit) {
        pid->integral = pid->integral_limit;
    } else if (pid->integral < -pid->integral_limit) {
        pid->integral = -pid->integral_limit;
    }
    
    float integral_term = pid->Ki * pid->integral;
    
    // Derivative term
    float derivative = (error - pid->prev_error) / dt;
    float derivative_term = pid->Kd * derivative;
    
    // Update previous error
    pid->prev_error = error;
    
    // Calculate output
    float output = proportional + integral_term + derivative_term;
    
    // Output limit
    if (output > pid->output_limit) {
        output = pid->output_limit;
    } else if (output < -pid->output_limit) {
        output = -pid->output_limit;
    }
    
    return output;
}

// Voltage loop calculation function
void VoltagePID_Calculate(VoltagePID* vpid, float dt) {
    // Calculate voltage error
    float voltage_error = vpid->voltage_setpoint - vpid->voltage_feedback;
    
    // Calculate current setpoint
    vpid->current_setpoint = PID_Calculate(&vpid->pid, voltage_error, dt);
    
    // Current setpoint limit (assuming max current is 10A)
    if (vpid->current_setpoint > 10.0f) {
        vpid->current_setpoint = 10.0f;
    } else if (vpid->current_setpoint < 0.0f) {
        vpid->current_setpoint = 0.0f;
    }
}

// Current loop calculation function
void CurrentPID_Calculate(CurrentPID* cpid, float dt) {
    // Calculate current error
    float current_error = cpid->current_setpoint - cpid->current_feedback;
    
    // Calculate duty cycle
    cpid->duty_cycle = PID_Calculate(&cpid->pid, current_error, dt);
    
    // Duty cycle limit between 0.0~1.0
    if (cpid->duty_cycle > 1.0f) {
        cpid->duty_cycle = 1.0f;
    } else if (cpid->duty_cycle < 0.0f) {
        cpid->duty_cycle = 0.0f;
    }
}

int main() {
    // Initialize voltage loop PID
    VoltagePID voltage_pid;
    PID_Init(&voltage_pid.pid, 0.5f, 0.1f, 0.01f, 5.0f, 5.0f); // Voltage loop parameters
    voltage_pid.voltage_setpoint = 12.0f;
    
    // Initialize current loop PID
    CurrentPID current_pid;
    PID_Init(&current_pid.pid, 0.8f, 0.2f, 0.02f, 3.0f, 1.0f); // Current loop parameters
    
    // Simulate control cycle
    float dt = 0.001f; // Control cycle 1ms
    
    // Test data
    float voltage_feedback[] = {11.5f, 11.8f, 11.9f};
    float current_feedback[] = {3.0f, 3.2f, 3.1f};
    
    printf("Cascade PID Controller Test\n");
    printf("===========================\n");
    
    for (int i = 0; i < 3; i++) {
        // Set feedback values
        voltage_pid.voltage_feedback = voltage_feedback[i];
        current_pid.current_feedback = current_feedback[i];
        
        // Voltage loop calculation
        VoltagePID_Calculate(&voltage_pid, dt);
        
        // Set current loop setpoint as voltage loop output
        current_pid.current_setpoint = voltage_pid.current_setpoint;
        
        // Current loop calculation
        CurrentPID_Calculate(&current_pid, dt);
        
        // Print results
        printf("Cycle %d:\n", i + 1);
        printf("  Voltage setpoint: %.2f V\n", voltage_pid.voltage_setpoint);
        printf("  Voltage feedback: %.2f V\n", voltage_pid.voltage_feedback);
        printf("  Voltage error: %.3f V\n", voltage_pid.voltage_setpoint - voltage_pid.voltage_feedback);
        printf("  Current setpoint: %.3f A\n", current_pid.current_setpoint);
        printf("  Current feedback: %.2f A\n", current_pid.current_feedback);
        printf("  Current error: %.3f A\n", current_pid.current_setpoint - current_pid.current_feedback);
        printf("  Final duty cycle: %.3f\n", current_pid.duty_cycle);
        printf("\n");
    }
    
    return 0;
}