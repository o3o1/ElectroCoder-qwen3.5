#include <stdio.h>

// PI Controller structure
typedef struct {
    float kp;           // Proportional gain
    float ki;           // Integral gain
    float integral;     // Integral accumulator
    float output_min;   // Minimum output limit
    float output_max;   // Maximum output limit
} PI_Controller;

// Initialize PI controller
void pi_init(PI_Controller* pi, float kp, float ki, float output_min, float output_max) {
    pi->kp = kp;
    pi->ki = ki;
    pi->integral = 0.0f;
    pi->output_min = output_min;
    pi->output_max = output_max;
}

// PI controller update function
float pi_update(PI_Controller* pi, float error, float dt) {
    // Proportional term
    float proportional = pi->kp * error;
    
    // Integral term with anti-windup
    pi->integral += pi->ki * error * dt;
    
    // Calculate total output
    float output = proportional + pi->integral;
    
    // Limit output
    if (output > pi->output_max) {
        output = pi->output_max;
        // Anti-windup: stop integration when output saturates
        pi->integral = pi->output_max - proportional;
    } else if (output < pi->output_min) {
        output = pi->output_min;
        // Anti-windup
        pi->integral = pi->output_min - proportional;
    }
    
    return output;
}

// Limit duty cycle to 0.1-0.9 range
float limit_duty(float duty) {
    if (duty < 0.1f) return 0.1f;
    if (duty > 0.9f) return 0.9f;
    return duty;
}

int main() {
    // Initialize PI controller
    PI_Controller pi;
    pi_init(&pi, 0.05f, 0.01f, -0.5f, 0.5f); // Gains and output limits
    
    float target_voltage = 24.0f; // Target output voltage
    float dt = 0.01f; // Control period in seconds (10ms)
    
    // Simulated ADC readings (starting below target and converging)
    float sampled_voltages[] = {23.1f, 23.8f, 24.2f, 24.0f, 24.1f};
    
    printf("Boost Converter Digital Control Simulation\n");
    printf("Target Voltage: %.1fV\n\n", target_voltage);
    
    // Simulate 5 control cycles
    for (int cycle = 0; cycle < 5; cycle++) {
        // 1. Read ADC (simulate voltage measurement)
        float sampled_voltage = sampled_voltages[cycle];
        
        // 2. Calculate error
        float error = target_voltage - sampled_voltage;
        
        // 3. Update PI controller
        float pi_output = pi_update(&pi, error, dt);
        
        // 4. Calculate duty cycle (base duty + PI correction)
        // For Boost converter: Vout = Vin/(1-D), so D = 1 - Vin/Vout
        // With Vin=12V, Vout=24V, ideal D = 0.5
        float base_duty = 0.5f; // Ideal duty for 12V->24V conversion
        float duty_cycle = base_duty + pi_output;
        
        // 5. Limit duty cycle
        duty_cycle = limit_duty(duty_cycle);
        
        // 6. Print results
        printf("Cycle %d: Sample=%.1fV, Error=%.1fV, PI Output=%.2f, Duty Cycle=%.2f\n",
               cycle + 1, sampled_voltage, error, pi_output, duty_cycle);
    }
    
    return 0;
}