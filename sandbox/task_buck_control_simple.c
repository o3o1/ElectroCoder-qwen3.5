#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Buck converter parameters
#define VIN 12.0f
#define VREF 5.0f
#define TS 0.001f

// PI controller parameters
#define KP 0.1f
#define KI 0.05f

// PI controller structure
typedef struct {
    float kp;
    float ki;
    float integral;
    float prev_error;
    float output_min;
    float output_max;
} PI_Controller;

// Initialize PI controller
void pi_init(PI_Controller* pi, float kp, float ki, float min, float max) {
    pi->kp = kp;
    pi->ki = ki;
    pi->integral = 0.0f;
    pi->prev_error = 0.0f;
    pi->output_min = min;
    pi->output_max = max;
}

// Update PI controller
float pi_update(PI_Controller* pi, float error) {
    // Proportional term
    float proportional = pi->kp * error;
    
    // Integral term with anti-windup
    pi->integral += pi->ki * error * TS;
    
    // Calculate output
    float output = proportional + pi->integral;
    
    // Clamp output to prevent windup
    if (output > pi->output_max) {
        output = pi->output_max;
        pi->integral = pi->output_max - proportional;
    } else if (output < pi->output_min) {
        output = pi->output_min;
        pi->integral = pi->output_min - proportional;
    }
    
    pi->prev_error = error;
    return output;
}

// Simulate ADC reading (Vout with noise)
int simulate_adc(float duty_cycle) {
    // Ideal Vout = Vin * D
    float ideal_vout = VIN * duty_cycle;
    
    // Add random noise [-0.1V, 0.1V]
    float noise = ((float)rand() / RAND_MAX) * 0.2f - 0.1f;
    float vout = ideal_vout + noise;
    
    // Convert to ADC value (voltage * 1000)
    int adc_value = (int)(vout * 1000.0f);
    
    // Clamp to reasonable range
    if (adc_value < 0) adc_value = 0;
    if (adc_value > 12000) adc_value = 12000;
    
    return adc_value;
}

// Convert ADC value back to voltage
float adc_to_voltage(int adc_value) {
    return adc_value / 1000.0f;
}

int main() {
    // Seed random number generator
    srand(time(NULL));
    
    // Initialize PI controller
    PI_Controller pi;
    pi_init(&pi, KP, KI, 0.05f, 0.95f);
    
    // Initial duty cycle
    float duty_cycle = 0.5f;
    
    printf("Buck Converter Digital Control Simulation\n");
    printf("Vin = %.1fV, Vref = %.1fV, Ts = %.3fs\n", VIN, VREF, TS);
    printf("PI: Kp = %.2f, Ki = %.2f\n", KP, KI);
    printf("Duty cycle range: %.2f - %.2f\n\n", 0.05f, 0.95f);
    
    printf("Cycle\tADC Value\tVout (V)\tError (V)\tDuty Cycle\n");
    printf("----------------------------------------------------------------\n");
    
    // Simulate 5 control cycles
    for (int cycle = 1; cycle <= 5; cycle++) {
        // Simulate ADC reading
        int adc_value = simulate_adc(duty_cycle);
        float vout = adc_to_voltage(adc_value);
        
        // Calculate error
        float error = VREF - vout;
        
        // Update PI controller to get new duty cycle
        float new_duty_cycle = pi_update(&pi, error);
        
        // Clamp duty cycle to safe range
        if (new_duty_cycle > 0.95f) new_duty_cycle = 0.95f;
        if (new_duty_cycle < 0.05f) new_duty_cycle = 0.05f;
        
        // Update duty cycle for next cycle
        duty_cycle = new_duty_cycle;
        
        // Print results
        printf("%d\t%d\t\t%.3f\t\t%.3f\t\t%.3f\n", 
               cycle, adc_value, vout, error, duty_cycle);
    }
    
    printf("\nSimulation completed.\n");
    
    return 0;
}