#include <stdio.h>

float incremental_pid(float target, float measure, float kp, float ki, float kd) {
    static float e_k_1 = 0.0f;
    static float e_k_2 = 0.0f;
    
    float e_k = target - measure;
    float delta_u = kp * (e_k - e_k_1) + ki * e_k + kd * (e_k - 2.0f * e_k_1 + e_k_2);
    
    e_k_2 = e_k_1;
    e_k_1 = e_k;
    
    return delta_u;
}

int main() {
    printf("Test case 1: target=10.0, Kp=0.8, Ki=0.2, Kd=0.1\n");
    float measurements1[] = {8.0f, 9.0f, 9.5f, 10.0f};
    for (int i = 0; i < 4; i++) {
        float delta_u = incremental_pid(10.0f, measurements1[i], 0.8f, 0.2f, 0.1f);
        printf("  measure=%.1f, delta_u(k)=%.6f\n", measurements1[i], delta_u);
    }
    
    printf("\nTest case 2: target=5.0, Kp=1.0, Ki=0.5, Kd=0.2\n");
    float measurements2[] = {2.0f, 3.0f, 4.0f, 5.0f};
    for (int i = 0; i < 4; i++) {
        float delta_u = incremental_pid(5.0f, measurements2[i], 1.0f, 0.5f, 0.2f);
        printf("  measure=%.1f, delta_u(k)=%.6f\n", measurements2[i], delta_u);
    }
    
    printf("\nTest case 3: target=0.0, Kp=0.5, Ki=0.1, Kd=0.05\n");
    float measurements3[] = {1.0f, 0.5f, 0.2f, 0.0f};
    for (int i = 0; i < 4; i++) {
        float delta_u = incremental_pid(0.0f, measurements3[i], 0.5f, 0.1f, 0.05f);
        printf("  measure=%.1f, delta_u(k)=%.6f\n", measurements3[i], delta_u);
    }
    
    return 0;
}