#include <stdio.h>
#include <math.h>

typedef struct {
    float kp;
    float ki;
    float kd;
    float integral_limit;
    float output_limit;
    float prev_error;
    float prev_error2;
} IncrementalPID;

void pid_init(IncrementalPID *pid, float kp, float ki, float kd, float integral_limit, float output_limit) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->integral_limit = integral_limit;
    pid->output_limit = output_limit;
    pid->prev_error = 0.0f;
    pid->prev_error2 = 0.0f;
}

float pid_update(IncrementalPID *pid, float setpoint, float measurement) {
    float error = setpoint - measurement;
    
    float current_ki = pid->ki;
    if (fabs(error) > pid->integral_limit) {
        current_ki = 0.0f;
    }
    
    float delta_u = pid->kp * (error - pid->prev_error) + 
                   current_ki * error + 
                   pid->kd * (error - 2.0f * pid->prev_error + pid->prev_error2);
    
    float output = delta_u;
    
    if (output > pid->output_limit) {
        output = pid->output_limit;
    } else if (output < -pid->output_limit) {
        output = -pid->output_limit;
    }
    
    pid->prev_error2 = pid->prev_error;
    pid->prev_error = error;
    
    return output;
}

int main() {
    IncrementalPID pid;
    pid_init(&pid, 1.0f, 0.1f, 0.05f, 5.0f, 10.0f);
    
    float setpoint = 10.0f;
    float measurements[] = {0.0f, 3.0f, 7.0f, 9.5f, 10.0f};
    int num_cycles = 5;
    
    printf("Incremental PID Controller Test:\n");
    printf("Setpoint: %.2f\n", setpoint);
    printf("Parameters: kp=%.2f, ki=%.2f, kd=%.2f, integral_limit=%.2f, output_limit=%.2f\n\n",
           pid.kp, pid.ki, pid.kd, pid.integral_limit, pid.output_limit);
    
    for (int i = 0; i < num_cycles; i++) {
        float output = pid_update(&pid, setpoint, measurements[i]);
        printf("Cycle %d: measurement=%.2f, output=%.2f\n", 
               i + 1, measurements[i], output);
    }
    
    return 0;
}