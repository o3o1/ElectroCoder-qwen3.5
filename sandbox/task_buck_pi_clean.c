#include <stdio.h>

#define KP 0.1
#define KI 0.01
#define D0 0.5
#define D_MIN 0.1
#define D_MAX 0.9
#define INTEGRAL_MIN -10.0
#define INTEGRAL_MAX 10.0

double buck_pi_control(double Vref, double Vfb, double *integral) {
    double error = Vref - Vfb;
    
    *integral += error;
    
    if (*integral > INTEGRAL_MAX) {
        *integral = INTEGRAL_MAX;
    } else if (*integral < INTEGRAL_MIN) {
        *integral = INTEGRAL_MIN;
    }
    
    double D = KP * error + KI * (*integral) + D0;
    
    if (D > D_MAX) {
        D = D_MAX;
    } else if (D < D_MIN) {
        D = D_MIN;
    }
    
    return D;
}

int main() {
    double Vref = 5.0;
    double integral = 0.0;
    
    printf("Buck降压转换器PI控制器测试\n");
    printf("Vref = %.2fV, Kp = %.2f, Ki = %.2f, D0 = %.2f\n\n", Vref, KP, KI, D0);
    printf("Vfb(V)\tError(V)\tIntegral\tDuty Cycle\n");
    printf("---------------------------------------------\n");
    
    for (double Vfb = 0.0; Vfb <= 5.0; Vfb += 1.0) {
        double D = buck_pi_control(Vref, Vfb, &integral);
        double error = Vref - Vfb;
        
        printf("%.2f\t%.2f\t\t%.4f\t\t%.4f\n", Vfb, error, integral, D);
    }
    
    printf("\n测试完成。\n");
    return 0;
}