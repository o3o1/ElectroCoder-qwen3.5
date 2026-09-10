#include <stdio.h>
#include <math.h>

float estimate_junction_temperature(float p_loss, float r_th, float t_ambient) {
    return t_ambient + p_loss * r_th;
}

int adjust_frequency(float t_junction, float t_max, int f_current) {
    if (t_junction < t_max - 10.0f) {
        return f_current;
    } else if (t_junction < t_max) {
        return (int)(f_current * 0.8f);
    } else {
        printf("OVERTEMP!\n");
        return (int)(f_current * 0.5f);
    }
}

int main() {
    // Test case 1: Normal operation
    float p_loss1 = 20.5f;
    float r_th1 = 0.8f;
    float t_amb1 = 30.0f;
    int f_curr1 = 20000;
    float t_max = 150.0f;
    
    float t_j1 = estimate_junction_temperature(p_loss1, r_th1, t_amb1);
    int f_adj1 = adjust_frequency(t_j1, t_max, f_curr1);
    printf("Test 1: P_loss=%.1f, R_th=%.1f, T_amb=%.1f => T_j=%.1f, F_adj=%d\n", 
           p_loss1, r_th1, t_amb1, t_j1, f_adj1);
    
    // Test case 2: Moderate temperature, frequency reduction
    float p_loss2 = 50.0f;
    float r_th2 = 1.2f;
    float t_amb2 = 40.0f;
    int f_curr2 = 25000;
    
    float t_j2 = estimate_junction_temperature(p_loss2, r_th2, t_amb2);
    int f_adj2 = adjust_frequency(t_j2, t_max, f_curr2);
    printf("Test 2: P_loss=%.1f, R_th=%.1f, T_amb=%.1f => T_j=%.1f, F_adj=%d\n", 
           p_loss2, r_th2, t_amb2, t_j2, f_adj2);
    
    // Test case 3: Over temperature, severe frequency reduction
    float p_loss3 = 100.0f;
    float r_th3 = 1.5f;
    float t_amb3 = 50.0f;
    int f_curr3 = 30000;
    
    float t_j3 = estimate_junction_temperature(p_loss3, r_th3, t_amb3);
    int f_adj3 = adjust_frequency(t_j3, t_max, f_curr3);
    printf("Test 3: P_loss=%.1f, R_th=%.1f, T_amb=%.1f => T_j=%.1f, F_adj=%d\n", 
           p_loss3, r_th3, t_amb3, t_j3, f_adj3);
    
    return 0;
}