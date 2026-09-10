#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define VIN 12.0
#define TARGET_VOUT 24.0
#define INITIAL_DUTY 0.5
#define DT 0.001
#define NOISE_AMPLITUDE 0.1

typedef struct {
    double Kp;
    double Ki;
    double Kd;
    double integral;
    double prev_error;
} PIDController;

double pid_update(PIDController *pid, double error, double dt) {
    double proportional = pid->Kp * error;
    
    pid->integral += error * dt;
    double integral = pid->Ki * pid->integral;
    
    double derivative = pid->Kd * (error - pid->prev_error) / dt;
    pid->prev_error = error;
    
    return proportional + integral + derivative;
}

double limit_duty(double duty) {
    if (duty < 0.05) return 0.05;
    if (duty > 0.95) return 0.95;
    return duty;
}

double simulate_vout(double duty, double noise) {
    return VIN / (1.0 - duty) + noise;
}

double generate_noise() {
    return ((double)rand() / RAND_MAX * 2.0 - 1.0) * NOISE_AMPLITUDE;
}

int main() {
    srand(time(NULL));
    
    PIDController pid = {
        .Kp = 0.1,
        .Ki = 0.05,
        .Kd = 0.01,
        .integral = 0.0,
        .prev_error = 0.0
    };
    
    double duty = INITIAL_DUTY;
    double time = 0.0;
    
    printf("Time(s)\tCurrent Vout(V)\tError(V)\tDuty\n");
    printf("---------------------------------------------\n");
    
    for (int i = 0; i < 5; i++) {
        double noise = generate_noise();
        double vout = simulate_vout(duty, noise);
        double error = TARGET_VOUT - vout;
        
        double pid_output = pid_update(&pid, error, DT);
        duty += pid_output;
        duty = limit_duty(duty);
        
        printf("%.4f\t%.4f\t\t%.4f\t\t%.4f\n", time, vout, error, duty);
        
        time += DT;
    }
    
    return 0;
}