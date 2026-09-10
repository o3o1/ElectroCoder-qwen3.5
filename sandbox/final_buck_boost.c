#include <stdio.h>
#include <math.h>

// System parameters
#define VIN 24.0
#define VREF -12.0
#define I_MAX 5.0
#define FSW 100000.0
#define T (1.0/FSW)
#define L 100e-6
#define C 470e-6
#define R_LOAD 6.0

// Controller parameters
#define KP_I 0.05
#define KP_V 0.1
#define KI_V 15.0
#define INTEGRAL_LIMIT 2.0
#define DUTY_MIN 0.0
#define DUTY_MAX 0.9

// Voltage PI controller
double voltage_pi_controller(double error, double *integral) {
    double integral_term = *integral + KI_V * error * T;
    
    // Integral anti-windup
    if (integral_term > INTEGRAL_LIMIT) integral_term = INTEGRAL_LIMIT;
    if (integral_term < -INTEGRAL_LIMIT) integral_term = -INTEGRAL_LIMIT;
    
    *integral = integral_term;
    
    double output = KP_V * error + integral_term;
    
    // Output saturation
    if (output > I_MAX) output = I_MAX;
    if (output < -I_MAX) output = -I_MAX;
    
    return output;
}

// Current P controller
double current_p_controller(double error) {
    return KP_I * error;
}

// PWM duty cycle calculation
double calculate_duty(double inductor_current, double voltage_pi_output) {
    double current_error = voltage_pi_output - inductor_current;
    double duty_increment = current_p_controller(current_error);
    
    double duty = 0.5 + duty_increment;
    
    // Duty cycle limiting
    if (duty > DUTY_MAX) duty = DUTY_MAX;
    if (duty < DUTY_MIN) duty = DUTY_MIN;
    
    return duty;
}

// Buck-Boost circuit state update
void update_circuit_state(double duty, double *Vout, double *I_L) {
    // Inductor current update
    double I_L_new = *I_L + (VIN * duty / L - (*Vout) * (1 - duty) / L) * T;
    
    // Output voltage update
    double Vout_new = *Vout + (*I_L * (1 - duty) - *Vout / R_LOAD) * T / C;
    
    // Current limiting
    if (I_L_new > I_MAX) I_L_new = I_MAX;
    if (I_L_new < -I_MAX) I_L_new = -I_MAX;
    
    *I_L = I_L_new;
    *Vout = Vout_new;
}

int main() {
    double Vout = 0.0;
    double I_L = 0.0;
    double integral = 0.0;
    
    printf("Buck-Boost Converter Dual-Loop Control Simulation\n");
    printf("Cycle\tVout(V)\t\tIL(A)\t\tDuty\n");
    printf("----------------------------------------\n");
    
    for (int cycle = 0; cycle < 1000; cycle++) {
        // 1) Calculate voltage error
        double error = VREF - Vout;
        
        // 2) Voltage PI controller
        double pi_output = voltage_pi_controller(error, &integral);
        
        // 3) Calculate duty cycle
        double duty = calculate_duty(I_L, pi_output);
        
        // 4) Update circuit state
        update_circuit_state(duty, &Vout, &I_L);
        
        // 5) Print every 100 cycles
        if ((cycle % 100 == 0) || cycle == 999) {
            printf("%d\t%.3f\t\t%.3f\t\t%.3f\n", cycle, Vout, I_L, duty);
        }
    }
    
    printf("----------------------------------------\n");
    printf("Simulation complete\n");
    
    return 0;
}