#include <stdio.h>
#include <math.h>

#define PI 3.14159265358979323846
#define TS 0.0001
#define F_GRID 50.0
#define A 1.0
#define INIT_PHASE 0.2
#define K_SOGI 1.414
#define KP 100.0
#define KI 1000.0
#define OMEGA_0 (2.0 * PI * F_GRID)

int main() {
    double sogi_u = 0.0;
    double sogi_u_quad = 0.0;
    double sogi_v1 = 0.0;
    double sogi_v2 = 0.0;
    
    double pll_error = 0.0;
    double pll_integral = 0.0;
    double omega_est = OMEGA_0;
    double theta_est = 0.0;
    
    printf("采样点, 输入电压, 估计频率(Hz), 估计相位(弧度)\n");
    
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