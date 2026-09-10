#include <stdio.h>
#include <math.h>

#define PWM_PERIOD 10000
#define DEADTIME_REG *(volatile uint32_t*)0x40000000

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

double calculate_zvs_deadtime(double Lr_uH, double Cr_nF, double Io, double Vin) {
    double Lr = Lr_uH * 1e-6;
    double Cr = Cr_nF * 1e-9;
    
    double T_resonant = M_PI * sqrt(Lr * Cr);
    double T_charge = (2 * Cr * Vin) / Io;
    
    double T_dead_seconds = T_resonant + T_charge;
    return T_dead_seconds * 1e9;
}

int check_zvs_condition(double T_actual_ns, double T_min_ns) {
    return (T_actual_ns >= T_min_ns) ? 1 : 0;
}

int main() {
    double test_cases[3][5] = {
        {50.0, 2.2, 5.0, 400.0, 150.0},
        {30.0, 1.5, 2.0, 300.0, 100.0},
        {80.0, 3.3, 8.0, 500.0, 200.0}
    };
    
    for (int i = 0; i < 3; i++) {
        double Lr = test_cases[i][0];
        double Cr = test_cases[i][1];
        double Io = test_cases[i][2];
        double Vin = test_cases[i][3];
        double T_actual = test_cases[i][4];
        
        double T_min = calculate_zvs_deadtime(Lr, Cr, Io, Vin);
        int zvs_result = check_zvs_condition(T_actual, T_min);
        
        printf("测试%d: Lr=%.1fμH, Cr=%.1fnF, Io=%.1fA, Vin=%.1fV, T_min=%.2fns, T_actual=%.0fns, ZVS=%s\n",
               i + 1, Lr, Cr, Io, Vin, T_min, T_actual, zvs_result ? "是" : "否");
    }
    
    return 0;
}