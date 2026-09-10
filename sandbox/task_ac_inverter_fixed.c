#include <stdio.h>
#include <math.h>

typedef struct {
    double Kp;
    double Ki;
    double Kd;
    double accum;
    double prev_error;
} PIDController;

double pid_update(PIDController* pid, double error, double dt) {
    double p_term = pid->Kp * error;
    
    pid->accum += error * dt;
    double i_term = pid->Ki * pid->accum;
    
    double d_term = 0.0;
    if (dt > 0.0) {
        d_term = pid->Kd * (error - pid->prev_error) / dt;
    }
    
    pid->prev_error = error;
    
    return p_term + i_term + d_term;
}

double clamp_frequency(double freq) {
    if (freq < 0.0) return 0.0;
    if (freq > 50.0) return 50.0;
    return freq;
}

int main() {
    PIDController pid = {2.0, 0.5, 0.1, 0.0, 0.0};
    
    double set_temp = 25.0;
    double actual_temp = 26.0;
    double dt = 0.1;
    
    printf("时间(s)\t设定温度(°C)\t实际温度(°C)\t计算频率(Hz)\n");
    printf("--------------------------------------------------------\n");
    
    for (int i = 0; i < 5; i++) {
        double time = i * dt;
        double error = set_temp - actual_temp;
        
        double raw_freq = pid_update(&pid, error, dt);
        double freq = clamp_frequency(raw_freq);
        
        printf("%.1f\t%.1f\t\t%.1f\t\t%.2f\n", 
               time, set_temp, actual_temp, freq);
        
        actual_temp = actual_temp - (freq / 50.0) * 0.3;
    }
    
    return 0;
}