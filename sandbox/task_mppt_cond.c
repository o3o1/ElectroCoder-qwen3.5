#include <stdio.h>
#include <math.h>

#define EPSILON 0.01

int mppt_incremental_conductance(double V, double I, double V_prev, double I_prev) {
    if (fabs(V_prev) < 1e-6) {
        return 0;
    }
    
    double dV = V - V_prev;
    double dI = I - I_prev;
    
    if (fabs(dV) < 1e-6) {
        return 0;
    }
    
    double dIdV = dI / dV;
    double dPdV = I + V * dIdV;
    
    if (fabs(dPdV) <= EPSILON) {
        return 0;
    } else if (dPdV > 0) {
        return 1;
    } else {
        return -1;
    }
}

int main() {
    printf("MPPT Incremental Conductance Algorithm Tests:\n\n");
    
    double V1 = 15.0, I1 = 8.0, V_prev1 = 14.5, I_prev1 = 8.2;
    int result1 = mppt_incremental_conductance(V1, I1, V_prev1, I_prev1);
    printf("Test 1: V=%.1f, I=%.1f, V_prev=%.1f, I_prev=%.1f => result=%d\n", 
           V1, I1, V_prev1, I_prev1, result1);
    
    double V2 = 18.0, I2 = 6.5, V_prev2 = 18.5, I_prev2 = 7.0;
    int result2 = mppt_incremental_conductance(V2, I2, V_prev2, I_prev2);
    printf("Test 2: V=%.1f, I=%.1f, V_prev=%.1f, I_prev=%.1f => result=%d\n", 
           V2, I2, V_prev2, I_prev2, result2);
    
    double V3 = 16.5, I3 = 7.8, V_prev3 = 16.5, I_prev3 = 7.8;
    int result3 = mppt_incremental_conductance(V3, I3, V_prev3, I_prev3);
    printf("Test 3: V=%.1f, I=%.1f, V_prev=%.1f, I_prev=%.1f => result=%d\n", 
           V3, I3, V_prev3, I_prev3, result3);
    
    double V4 = 17.0, I4 = 7.2, V_prev4 = 16.9, I_prev4 = 7.21;
    int result4 = mppt_incremental_conductance(V4, I4, V_prev4, I_prev4);
    printf("Test 4: V=%.1f, I=%.1f, V_prev=%.1f, I_prev=%.1f => result=%d\n", 
           V4, I4, V_prev4, I_prev4, result4);
    
    return 0;
}