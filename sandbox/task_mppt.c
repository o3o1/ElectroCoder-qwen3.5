#include <stdio.h>
#include <math.h>

float mppt_perturb_observe(float V, float I, float V_prev, float P_prev, float D, float delta_D) {
    float P = V * I;
    
    if (P > P_prev) {
        if (V > V_prev) {
            D = D + delta_D;
        } else {
            D = D - delta_D;
        }
    } else {
        if (V > V_prev) {
            D = D - delta_D;
        } else {
            D = D + delta_D;
        }
    }
    
    if (D < 0.0f) D = 0.0f;
    if (D > 1.0f) D = 1.0f;
    
    return D;
}

int main() {
    float D = 0.3f;
    float delta_D = 0.05f;
    float V_prev = 0.0f;
    float P_prev = 0.0f;
    
    printf("MPPT Perturb & Observe Simulation\n");
    printf("=================================\n");
    
    for (int i = 1; i <= 10; i++) {
        float V = D * 5.0f;
        float I = 10.0f - 2.0f * V;
        float P = V * I;
        
        D = mppt_perturb_observe(V, I, V_prev, P_prev, D, delta_D);
        
        printf("Iter %d: V=%.2fV, P=%.2fW, D=%.2f\n", i, V, P, D);
        
        V_prev = V;
        P_prev = P;
    }
    
    return 0;
}