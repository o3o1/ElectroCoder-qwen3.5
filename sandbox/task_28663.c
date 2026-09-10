#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define PI 3.14159265358979323846

double calculate_power_factor(double voltage_angle, double current_angle) {
    double voltage_rad = voltage_angle * PI / 180.0;
    double current_rad = current_angle * PI / 180.0;
    double phase_diff = fabs(voltage_rad - current_rad);
    return cos(phase_diff);
}

double closed_loop_control(double target_pf, double current_pf, double kp) {
    if (current_pf < target_pf) {
        return kp * (target_pf - current_pf);
    }
    return 0.0;
}

int main() {
    double target_pf = 0.99;
    double kp = 0.5;
    
    double test_data[5][2] = {
        {0, 10},
        {5, 15},
        {10, 18},
        {15, 20},
        {20, 22}
    };
    
    printf("Rectifier Power Factor Closed-loop Control Simulation\n");
    printf("Target Power Factor: %.2f\n", target_pf);
    printf("KP: %.1f\n\n", kp);
    printf("Voltage Angle(deg)\tCurrent Angle(deg)\tCurrent PF\tAdjustment\tStatus\n");
    printf("----------------------------------------------------------------------------\n");
    
    for (int i = 0; i < 5; i++) {
        double voltage_angle = test_data[i][0];
        double current_angle = test_data[i][1];
        double current_pf = calculate_power_factor(voltage_angle, current_angle);
        double adjustment = closed_loop_control(target_pf, current_pf, kp);
        const char* status = (current_pf >= target_pf) ? "OK" : "NEED ADJUST";
        
        printf("%.1f\t\t\t%.1f\t\t\t%.4f\t\t%.4f\t\t%s\n",
               voltage_angle, current_angle, current_pf, adjustment, status);
    }
    
    return 0;
}