#include <stdio.h>

float incremental_pid(float setpoint, float measured, float kp, float ki, float kd) {
    static float e_k_1 = 0.0f;
    static float e_k_2 = 0.0f;
    
    float e_k = setpoint - measured;
    
    float delta_u = kp * (e_k - e_k_1) + ki * e_k + kd * (e_k - 2.0f * e_k_1 + e_k_2);
    
    e_k_2 = e_k_1;
    e_k_1 = e_k;
    
    return delta_u;
}

int main() {
    float kp = 1.2f;
    float ki = 0.5f;
    float kd = 0.1f;
    float setpoint = 10.0f;
    
    float measurements[] = {10.0f, 9.5f, 9.0f, 8.8f, 8.5f};
    int num_cycles = 5;
    
    for (int i = 0; i < num_cycles; i++) {
        float measured = measurements[i];
        float error = setpoint - measured;
        float delta_u = incremental_pid(setpoint, measured, kp, ki, kd);
        
        printf("周期%d: 测量值=%.3f, 误差=%.3f, Δu=%.3f\n", 
               i + 1, measured, error, delta_u);
    }
    
    return 0;
}