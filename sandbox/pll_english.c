#include <stdio.h>
#include <math.h>

int main() {
    double PI = 3.14159265358979323846;
    double TS = 0.0001;
    double F_GRID = 50.0;
    double A = 1.0;
    double INIT_PHASE = 0.2;
    double K_SOGI = 1.414;
    double KP = 100.0;
    double KI = 1000.0;
    double OMEGA_0 = 2.0 * PI * F_GRID;
    
    double sogi_u = 0.0;
    double sogi_u_quad = 0.0;
    double sogi_v1 = 0.0;
    double sogi_v2 = 0.0;
    
    double pll_error = 0.0;
    double pll_integral = 0.0;
    double omega_est = OMEGA_0;
    double theta_est = 0.0;
    
    printf("Sample, Input Voltage, Estimated Frequency(Hz), Estimated Phase(rad)\n");
    
    for (int n = 0; n < 1000; n++) {
        double t = n * TS;
        double u_input = A * sin(2.0 * PI * F_GRID * t + INIT_PHASE);
        
        double k = K_SOGI;
        double v1_new = sogi_v1 + TS * (omega_est * sogi_v2 - k * omega_est * (sogi_v1 - u_input));
        double v2_new = sogi_v2 + TS * (-omega_est * sogi_v1);
        
        sogi_v1 = v1_new;
        sogi_v2 = v2_new;
        sogi_u = sogi_v1;
        sogi_u_quad = sogi_v2;
        
        pll_error = sogi_u * sogi_u_quad;
        pll_integral += pll_error * TS;
        omega_est = OMEGA_0 + KP * pll_error + KI * pll_integral;
        theta_est += omega_est * TS;
        
        if (theta_est > 2.0 * PI) {
            theta_est -= 2.0 * PI;
        }
        if (theta_est < 0) {
            theta_est += 2.0 * PI;
        }
        
        if (n < 10) {
            printf("%d, %.6f, %.6f, %.6f\n", 
                   n, 
                   u_input, 
                   omega_est / (2.0 * PI), 
                   theta_est);
        }
    }
    
    return 0;
}