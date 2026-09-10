#include <stdio.h>
#include <math.h>

#define MAX_DUTY_CYCLE 0.85

double calculate_duty_cycle(double vin, double vout_target) {
    if (vin <= 0 || vout_target <= 0) {
        return 0.0;
    }
    
    double d = 1.0 - (vin / vout_target);
    
    if (d < 0.0) {
        return 0.0;
    }
    
    if (d > MAX_DUTY_CYCLE) {
        return MAX_DUTY_CYCLE;
    }
    
    return d;
}

double pi_controller(double error, double *integral, double kp, double ki) {
    *integral += error;
    return (kp * error) + (ki * (*integral));
}

int main() {
    double vin = 12.0;
    double vout_target = 24.0;
    double vout_actual = 23.5;
    
    double initial_duty = calculate_duty_cycle(vin, vout_target);
    printf("初始占空比: %.4f\n", initial_duty);
    
    double error = vout_target - vout_actual;
    printf("电压误差: %.4f V\n", error);
    
    double integral = 0.0;
    double kp = 0.01;
    double ki = 0.001;
    
    double duty_increment = pi_controller(error, &integral, kp, ki);
    printf("PI调整后的占空比增量: %.6f\n", duty_increment);
    
    double new_duty = initial_duty + duty_increment;
    if (new_duty > MAX_DUTY_CYCLE) {
        new_duty = MAX_DUTY_CYCLE;
    }
    if (new_duty < 0.0) {
        new_duty = 0.0;
    }
    printf("调整后的占空比: %.4f\n", new_duty);
    
    return 0;
}