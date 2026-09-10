#include <stdio.h>
#include <math.h>

typedef struct {
    float Kp;
    float Ki;
    float Kd;
    float integral;
    float prev_error;
    float integral_limit;
    float output_limit;
    float error_threshold;
} PID_Controller;

void PID_Init(PID_Controller *pid, float kp, float ki, float kd, float int_limit, float out_limit, float err_thresh) {
    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
    pid->integral_limit = int_limit;
    pid->output_limit = out_limit;
    pid->error_threshold = err_thresh;
}

float PID_Update(PID_Controller *pid, float setpoint, float measurement, float dt) {
    float error = setpoint - measurement;
    
    if (fabs(error) > pid->error_threshold) {
        pid->integral = 0.0f;
    } else {
        float integral_update = pid->Ki * error * dt;
        float new_integral = pid->integral + integral_update;
        
        if (fabs(new_integral) <= pid->integral_limit) {
            pid->integral = new_integral;
        }
    }
    
    float derivative = (error - pid->prev_error) / dt;
    pid->prev_error = error;
    
    float output = pid->Kp * error + pid->integral + pid->Kd * derivative;
    
    if (output > pid->output_limit) {
        output = pid->output_limit;
    } else if (output < -pid->output_limit) {
        output = -pid->output_limit;
    }
    
    return output;
}

int main() {
    PID_Controller pid;
    PID_Init(&pid, 1.0f, 0.5f, 0.1f, 2.0f, 5.0f, 3.0f);
    
    float setpoints[] = {10.0f, 10.0f, 10.0f, 10.0f, 10.0f};
    float measurements[] = {0.0f, 2.0f, 4.0f, 8.0f, 12.0f};
    float dt = 0.1f;
    
    printf("PID Anti-Windup Controller Test\n");
    printf("Kp=%.1f, Ki=%.1f, Kd=%.1f, IntLimit=%.1f, OutLimit=%.1f, ErrThresh=%.1f\n\n",
           pid.Kp, pid.Ki, pid.Kd, pid.integral_limit, pid.output_limit, pid.error_threshold);
    
    for (int i = 0; i < 5; i++) {
        float output = PID_Update(&pid, setpoints[i], measurements[i], dt);
        printf("Cycle %d: Setpoint=%.1f, Measurement=%.1f, Error=%.1f, Output=%.3f, Integral=%.3f\n",
               i+1, setpoints[i], measurements[i], setpoints[i]-measurements[i], output, pid.integral);
    }
    
    return 0;
}