#include <stdio.h>
#include <math.h>

#define VIN 24.0
#define VREF -12.0
#define I_MAX 5.0
#define FSW 100000.0
#define T (1.0/FSW)
#define L 100e-6
#define C 470e-6
#define R_LOAD 6.0

#define KP_I 0.05
#define KP_V 0.1
#define KI_V 15.0
#define INTEGRAL_LIMIT 2.0
#define DUTY_MIN 0.0
#define DUTY_MAX 0.9

double voltage_pi_controller(double error, double *integral) {
    double integral_term = *integral + KI_V * error * T;
    
    if (integral_term > INTEGRAL_LIMIT) integral_term = INTEGRAL_LIMIT;
    if (integral_term < -INTEGRAL_LIMIT) integral_term = -INTEGRAL_LIMIT;
    
    *integral = integral_term;
    
    double output = KP_V * error + integral_term;
    
    if (output > I_MAX) output = I_MAX;
    if (output < -I_MAX) output = -I_MAX;
    
    return output;
}

double current_p_controller(double error) {
    return KP_I * error;
}

double calculate_duty(double inductor_current, double voltage_pi_output) {
    double current_error = voltage_pi_output - inductor_current;
    double duty_increment = current_p_controller(current_error);
    
    double duty = 0.5 + duty_increment;
    
    if (duty > DUTY_MAX) duty = DUTY_MAX;
    if (duty < DUTY_MIN) duty = DUTY_MIN;
    
    return duty;
}

void update_circuit_state(double duty, double *Vout, double *I_L) {
    double I_L_new = *I_L + (VIN * duty / L - (*Vout) * (1 - duty) / L) * T;
    double Vout_new = *Vout + (*I_L * (1 - duty) - *Vout / R_LOAD) * T / C;
    
    if (I_L_new > I_MAX) I_L_new = I_MAX;
    if (I_L_new < -I_MAX) I_L_new = -I_MAX;
    
    *I_L = I_L_new;
    *Vout = Vout_new;
}

int main() {
    double Vout = 0.0;
    double I_L = 0.0;
    double integral = 0.0;
    
    printf("Buck-Boost变换器双环控制仿真\n");
    printf("周期\t输出电压(V)\t电感电流(A)\t占空比\n");
    printf("------------------------------------------------\n");
    
    for (int cycle = 0; cycle < 1000; cycle++) {
        double error = VREF - Vout;
        
        double pi_output = voltage_pi_controller(error, &integral);
        
        double duty = calculate_duty(I_L, pi_output);
        
        update_circuit_state(duty, &Vout, &I_L);
        
        if ((cycle % 100 == 0) || cycle == 999) {
            printf("%d\t%.3f\t\t%.3f\t\t%.3f\n", cycle, Vout, I_L, duty);
        }
    }
    
    printf("------------------------------------------------\n");
    printf("仿真完成\n");
    
    return 0;
}