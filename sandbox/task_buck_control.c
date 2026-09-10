#include <stdio.h>
#include <math.h>

#define Kp 0.05
#define Ki 0.001
#define DUTY_MAX 0.95
#define DUTY_MIN 0.0

double buck_control(double Vref, double Vin, double Vout, double Iload, double* integral_err) {
    double error = Vref - Vout;
    
    double D_ff = Vref / Vin;
    
    double D_fb = Kp * error + Ki * (*integral_err);
    
    *integral_err += error;
    
    double D = D_ff + D_fb;
    
    if (D > DUTY_MAX) {
        D = DUTY_MAX;
    } else if (D < DUTY_MIN) {
        D = DUTY_MIN;
    }
    
    return D;
}

int main() {
    printf("=== Buck变换器动态响应优化控制算法测试 ===\n\n");
    
    double integral_err = 0.0;
    
    printf("=== 稳态测试 ===\n");
    double Vin = 12.0;
    double Vref = 5.0;
    double Vout = 4.95;
    double Iload = 1.0;
    
    for (int i = 0; i < 5; i++) {
        double D = buck_control(Vref, Vin, Vout, Iload, &integral_err);
        printf("Step %d: Vout=%.2fV, D=%.4f\n", i+1, Vout, D);
        Vout += 0.01;
    }
    
    printf("\n=== 输入电压突变测试 ===\n");
    integral_err = 0.0;
    Vin = 12.0;
    Vref = 5.0;
    Vout = 5.0;
    Iload = 1.0;
    
    double D_before = buck_control(Vref, Vin, Vout, Iload, &integral_err);
    printf("Before: Vin=%.1fV, D=%.4f\n", Vin, D_before);
    
    Vin = 15.0;
    double D_after = buck_control(Vref, Vin, Vout, Iload, &integral_err);
    printf("After:  Vin=%.1fV, D=%.4f\n", Vin, D_after);
    
    printf("\n=== 负载突变测试 ===\n");
    integral_err = 0.0;
    Vin = 12.0;
    Vref = 5.0;
    Vout = 5.0;
    Iload = 1.0;
    
    D_before = buck_control(Vref, Vin, Vout, Iload, &integral_err);
    printf("Before: Iload=%.1fA, D=%.4f\n", Iload, D_before);
    
    Iload = 2.0;
    D_after = buck_control(Vref, Vin, Vout, Iload, &integral_err);
    printf("After:  Iload=%.1fA, D=%.4f\n", Iload, D_after);
    
    return 0;
}