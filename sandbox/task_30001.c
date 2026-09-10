#include <stdio.h>
#include <math.h>

typedef struct {
    float target_position;
    float actual_position;
    float error;
    float Kp, Ki, Kd;
    float Kff;
    float integral;
    float last_error;
} MotorControl;

void motor_init(MotorControl* mc, float Kp, float Ki, float Kd, float Kff) {
    mc->Kp = Kp;
    mc->Ki = Ki;
    mc->Kd = Kd;
    mc->Kff = Kff;
    mc->target_position = 0.0f;
    mc->actual_position = 0.0f;
    mc->error = 0.0f;
    mc->integral = 0.0f;
    mc->last_error = 0.0f;
}

float pid_calculate(MotorControl* mc, float dt) {
    float proportional = mc->Kp * mc->error;
    mc->integral += mc->error * dt;
    float integral = mc->Ki * mc->integral;
    float derivative = mc->Kd * (mc->error - mc->last_error) / dt;
    mc->last_error = mc->error;
    return proportional + integral + derivative;
}

float feedforward_calculate(MotorControl* mc, float target_velocity) {
    return mc->Kff * target_velocity;
}

void motor_update(MotorControl* mc, float dt, float target_vel) {
    mc->error = mc->target_position - mc->actual_position;
    float pid_output = pid_calculate(mc, dt);
    float ff_output = feedforward_calculate(mc, target_vel);
    float control_output = pid_output + ff_output;
    float disturbance = 0.1f;
    mc->actual_position += (control_output + disturbance) * dt;
}

int main() {
    MotorControl motor;
    motor_init(&motor, 2.0f, 0.5f, 0.1f, 0.3f);
    
    float dt = 0.01f;
    float prev_target = 0.0f;
    
    for (int i = 1; i <= 5; i++) {
        motor.target_position = i * 20.0f;
        float target_vel = (motor.target_position - prev_target) / dt;
        prev_target = motor.target_position;
        
        motor_update(&motor, dt, target_vel);
        
        printf("Cycle[%d] Target:%.2f Actual:%.2f Error:%.2f\n", 
               i, motor.target_position, motor.actual_position, motor.error);
    }
    
    return 0;
}