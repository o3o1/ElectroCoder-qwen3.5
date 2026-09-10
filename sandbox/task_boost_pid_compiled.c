#include <stdio.h>
#include <math.h>

#define VIN 12.0
#define VOUT_REF 24.0
#define L 100e-6
#define C 470e-6
#define R 10.0
#define FSW 50000.0
#define TS (1.0/FSW)

#define KP 0.01
#define KI 0.5
#define KD 0.0001

#define DUTY_MIN 0.1
#define DUTY_MAX 0.9

int main() {
    double iL = 0.0;
    double vout = 0.0;
    double duty = 0.5;
    double e_integral = 0.0;
    double e_prev = 0.0;
    
    int total_periods = 1000;
    int print_interval = 100;
    
    for (int period = 1; period <= total_periods; period++) {
        double error = VOUT_REF - vout;
        
        e_integral += error * TS;
        double e_derivative = (error - e_prev) / TS;
        
        duty = KP * error + KI * e_integral + KD * e_derivative;
        
        if (duty < DUTY_MIN) duty = DUTY_MIN;
        if (duty > DUTY_MAX) duty = DUTY_MAX;
        
        iL = iL + (VIN - (1.0 - duty) * vout) * TS / L;
        vout = vout + ((1.0 - duty) * iL - vout / R) * TS / C;
        
        e_prev = error;
        
        if (period % print_interval == 0) {
            printf("Period %d: Vout=%.2fV, Duty=%.3f, Error=%.2fV\n", 
                   period, vout, duty, error);
        }
    }
    
    double steady_state_error_percent = fabs((VOUT_REF - vout) / VOUT_REF) * 100.0;
    printf("Final Vout=%.2fV, Steady-state error=%.2f%%\n", 
           vout, steady_state_error_percent);
    
    return 0;
}