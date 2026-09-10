#include <stdio.h>
#include <math.h>

typedef struct {
    double kp;
    double ki;
    double kd;
    double integral;
    double prev_error;
    double dead_zone;
} DeadZonePID;

double pid_update(DeadZonePID *pid, double setpoint, double measurement, double dt) {
    double error = setpoint - measurement;
    
    double p_term = pid->kp * error;
    
    double d_term = 0.0;
    if (dt > 0.0) {
        d_term = pid->kd * (error - pid->prev_error) / dt;
    }
    
    if (fabs(error) >= pid->dead_zone) {
        pid->integral += pid->ki * error * dt;
    }
    
    double output = p_term + pid->integral + d_term;
    
    pid->prev_error = error;
    
    return output;
}

int main() {
    DeadZonePID pid = {
        .kp = 1.0,
        .ki = 0.5,
        .kd = 0.1,
        .integral = 0.0,
        .prev_error = 0.0,
        .dead_zone = 0.05
    };
    
    double setpoint = 1.0;
    double measurement = 0.0;
    double dt = 0.01;
    
    for (int i = 0; i < 100; i++) {
        double time = i * dt;
        double error = setpoint - measurement;
        double output = pid_update(&pid, setpoint, measurement, dt);
        
        measurement += (output - measurement) * dt;
        
        printf("%.3f %.3f %.3f %.3f %.3f\n", 
               time, setpoint, measurement, error, output);
    }
    
    return 0;
}