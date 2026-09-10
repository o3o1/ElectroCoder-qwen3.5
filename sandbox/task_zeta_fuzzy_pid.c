#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define RULE_SIZE 5

typedef struct {
    float kp, ki, kd;
    float error, last_error;
    float integral, derivative;
    float temperature;
    float rule_base[RULE_SIZE][RULE_SIZE];
} FuzzyPID;

void fuzzy_pid_init(FuzzyPID *pid) {
    pid->kp = 1.0f;
    pid->ki = 0.1f;
    pid->kd = 0.05f;
    pid->error = 0.0f;
    pid->last_error = 0.0f;
    pid->integral = 0.0f;
    pid->derivative = 0.0f;
    pid->temperature = 25.0f;
    
    float base_rules[RULE_SIZE][RULE_SIZE] = {
        {-0.2f, -0.15f, -0.1f, -0.05f, 0.0f},
        {-0.15f, -0.1f, -0.05f, 0.0f, 0.05f},
        {-0.1f, -0.05f, 0.0f, 0.05f, 0.1f},
        {-0.05f, 0.0f, 0.05f, 0.1f, 0.15f},
        {0.0f, 0.05f, 0.1f, 0.15f, 0.2f}
    };
    
    for (int i = 0; i < RULE_SIZE; i++) {
        for (int j = 0; j < RULE_SIZE; j++) {
            pid->rule_base[i][j] = base_rules[i][j];
        }
    }
}

float fuzzy_inference(float error, float d_error, float temperature) {
    float e_norm = fmaxf(-2.0f, fminf(2.0f, error));
    float de_norm = fmaxf(-2.0f, fminf(2.0f, d_error));
    
    int e_idx = (int)((e_norm + 2.0f) / 4.0f * (RULE_SIZE - 1));
    int de_idx = (int)((de_norm + 2.0f) / 4.0f * (RULE_SIZE - 1));
    
    e_idx = fmaxf(0, fminf(RULE_SIZE - 1, e_idx));
    de_idx = fmaxf(0, fminf(RULE_SIZE - 1, de_idx));
    
    float base_adjust = 0.0f;
    
    if (e_idx >= 0 && e_idx < RULE_SIZE && de_idx >= 0 && de_idx < RULE_SIZE) {
        base_adjust = 0.0f;
    }
    
    float temp_factor = 1.0f + (temperature - 25.0f) / 100.0f;
    
    return base_adjust * temp_factor;
}

float fuzzy_pid_update(FuzzyPID *pid, float setpoint, float measured, float dt) {
    pid->last_error = pid->error;
    pid->error = setpoint - measured;
    
    float d_error = (pid->error - pid->last_error) / dt;
    
    float adjust = fuzzy_inference(pid->error, d_error, pid->temperature);
    
    float kp_adj = pid->kp * (1.0f + adjust);
    float ki_adj = pid->ki * (1.0f + adjust * 0.5f);
    float kd_adj = pid->kd * (1.0f + adjust * 0.3f);
    
    pid->integral += pid->error * dt;
    pid->derivative = d_error;
    
    float output = kp_adj * pid->error + ki_adj * pid->integral + kd_adj * pid->derivative;
    
    return output;
}

int main() {
    FuzzyPID pid;
    fuzzy_pid_init(&pid);
    
    float setpoint = 5.0f;
    float measured = 0.0f;
    float dt = 0.1f;
    
    for (int i = 0; i < 5; i++) {
        float time = (i + 1) * dt;
        pid.temperature = 25.0f + i * 15.0f;
        
        float output = fuzzy_pid_update(&pid, setpoint, measured, dt);
        
        measured += output * dt * 0.1f;
        
        printf("Time=%.1fs, Temp=%.1f°C, Output=%.2fV\n", time, pid.temperature, output);
    }
    
    return 0;
}