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
    float setpoint = 100.0f;
    float measured = 0.0f;
    float kp = 0.8f;
    float ki = 0.05f;
    float kd = 0.1f;
    
    for (int i = 1; i <= 5; i++) {
        float delta_u = incremental_pid(setpoint, measured, kp, ki, kd);
        measured += delta_u;
        
        printf("Loop %d: measured=%.2f, delta_u=%.2f\n", i, measured, delta_u);
    }
    
    return 0;
}