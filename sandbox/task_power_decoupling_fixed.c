#include <stdio.h>
#include <math.h>

#define L 0.001
#define OMEGA 314.16
#define KP 10.0
#define KI 100.0
#define DT 0.0001

void power_decoupling(double v_d, double v_q, double i_d_ref, double i_q_ref, double *u_d, double *u_q) {
    static double integral_d = 0.0;
    static double integral_q = 0.0;
    
    double i_d_meas = 0.0;
    double i_q_meas = 0.0;
    
    double error_d = i_d_ref - i_d_meas;
    double error_q = i_q_ref - i_q_meas;
    
    integral_d += error_d * DT;
    integral_q += error_q * DT;
    
    *u_d = v_d - OMEGA * L * i_q_ref + KP * error_d + KI * integral_d;
    *u_q = v_q + OMEGA * L * i_d_ref + KP * error_q + KI * integral_q;
}

int main() {
    double u_d, u_q;
    
    printf("Active/Reactive Power Decoupling Control Algorithm Test\n");
    printf("=======================================================\n\n");
    
    printf("1. Active power control test (i_d_ref = 100.0, i_q_ref = 0.0):\n");
    power_decoupling(311.0, 0.0, 100.0, 0.0, &u_d, &u_q);
    printf("   u_d = %.6f V\n", u_d);
    printf("   u_q = %.6f V\n\n", u_q);
    
    printf("2. Reactive power control test (i_d_ref = 0.0, i_q_ref = 50.0):\n");
    power_decoupling(311.0, 0.0, 0.0, 50.0, &u_d, &u_q);
    printf("   u_d = %.6f V\n", u_d);
    printf("   u_q = %.6f V\n\n", u_q);
    
    printf("3. Combined control test (i_d_ref = 80.0, i_q_ref = 30.0):\n");
    power_decoupling(311.0, 0.0, 80.0, 30.0, &u_d, &u_q);
    printf("   u_d = %.6f V\n", u_d);
    printf("   u_q = %.6f V\n\n", u_q);
    
    printf("4. Second call to verify integral accumulation:\n");
    power_decoupling(311.0, 0.0, 80.0, 30.0, &u_d, &u_q);
    printf("   u_d = %.6f V\n", u_d);
    printf("   u_q = %.6f V\n", u_q);
    
    return 0;
}