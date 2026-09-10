#include <stdio.h>
#include <math.h>

double calculate_duty_cycle(double vin, double vout_target) {
    if (vout_target <= vin) {
        return 0.0;
    }
    double d = 1.0 - (vin / vout_target);
    if (d > 0.95) {
        return 0.95;
    }
    if (d < 0.0) {
        return 0.0;
    }
    return d;
}

double boost_control(double vin_measured, double vout_measured, double vout_target, double kp) {
    double error = vout_target - vout_measured;
    double delta_d = kp * error;
    double base_d = calculate_duty_cycle(vin_measured, vout_target);
    double d = base_d + delta_d;
    
    if (d > 0.95) {
        return 0.95;
    }
    if (d < 0.05) {
        return 0.05;
    }
    return d;
}

int main() {
    printf("Boost converter digital control algorithm test\n");
    printf("==============================================\n\n");
    
    printf("Test a) calculate_duty_cycle test:\n");
    double vin1 = 12.0;
    double vout_target1 = 24.0;
    double d1 = calculate_duty_cycle(vin1, vout_target1);
    printf("Vin=%.2fV, Vout_target=%.2fV\n", vin1, vout_target1);
    printf("Calculated duty cycle D = %.4f (%.2f%%)\n", d1, d1 * 100.0);
    printf("Verification: Vout = Vin/(1-D) = %.2f/(1-%.4f) = %.2fV\n\n", vin1, d1, vin1/(1.0-d1));
    
    printf("Test b) boost_control test:\n");
    
    printf("Scene 1: Vin=12V, Vout_measured=22V, Vout_target=24V, kp=0.01\n");
    double d_scene1 = boost_control(12.0, 22.0, 24.0, 0.01);
    printf("Calculated duty cycle D = %.4f (%.2f%%)\n", d_scene1, d_scene1 * 100.0);
    
    printf("\nScene 2: Vin=12V, Vout_measured=26V, Vout_target=24V, kp=0.01\n");
    double d_scene2 = boost_control(12.0, 26.0, 24.0, 0.01);
    printf("Calculated duty cycle D = %.4f (%.2f%%)\n", d_scene2, d_scene2 * 100.0);
    
    printf("\nScene 3: Vin=12V, Vout_measured=15V, Vout_target=24V, kp=0.02\n");
    double d_scene3 = boost_control(12.0, 15.0, 24.0, 0.02);
    printf("Calculated duty cycle D = %.4f (%.2f%%)\n", d_scene3, d_scene3 * 100.0);
    
    return 0;
}