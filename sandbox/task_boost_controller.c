#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Mock hardware registers
#define ADC_VALUE_REG (*((volatile unsigned int*)0x40000000))
#define PWM_DUTY_REG  (*((volatile unsigned int*)0x40000004))

// PI Controller structure
typedef struct {
    float kp;           // Proportional gain
    float ki;           // Integral gain
    float integral;     // Integral accumulator
    float output_min;   // Minimum output limit
    float output_max;   // Maximum output limit
    float duty_min;     // Minimum duty cycle (0.1)
    float duty_max;     // Maximum duty cycle (0.9)
} PI_Controller;

// Initialize PI controller
void pi_init(PI_Controller* pi, float kp, float ki, float output_min, float output_max) {
    pi->kp = kp;
    pi->ki = ki;
    pi->integral = 0.0f;
    pi->output_min = output_min;
    pi->output_max = output_max;
    pi->duty_min = 0.1f;
    pi->duty_max = 0.9f;
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

// Convert duty cycle to PWM register value (assuming 16-bit PWM)
unsigned int duty_to_pwm(float duty) {
    // Limit duty cycle to 0.1-0.9 range
    if (duty < 0.1f) duty = 0.1f;
    if (duty > 0.9f) duty = 0.9f;
    
    // Convert to 16-bit PWM value (0-65535)
    return (unsigned int)(duty * 65535.0f);
}

// Mock ADC sampling function (simulates reading voltage)
float read_adc_voltage(void) {
    // In real hardware, this would read ADC_VALUE_REG and convert to voltage
    // For simulation, we'll return a mock value
    static float mock_voltages[] = {23.1f, 23.8f, 24.2f, 24.0f, 24.1f};
    static int index = 0;
    
    if (index < 5) {
        return mock_voltages[index++];
    }
    return 24.0f; // Default value
}

// Set PWM duty cycle
void set_pwm_duty(float duty) {
    // Limit duty cycle
    if (duty < 0.1f) duty = 0.1f;
    if (duty > 0.9f) duty = 0.9f;
    
    // Convert to PWM register value and write to mock register
    PWM_DUTY_REG = duty_to_pwm(duty);
}

int main() {
    // Initialize PI controller
    PI_Controller pi;
    pi_init(&pi, 0.05f, 0.01f, -0.5f, 0.5f); // Gains and output limits
    
    float target_voltage = 24.0f; // Target output voltage
    float dt = 0.01f; // Control period in seconds (10ms)
    
    printf("Boost Converter Digital Control Simulation\n");
    printf("Target Voltage: %.1fV\n\n", target_voltage);
    
    // Simulate 5 control cycles
    for (int cycle = 1; cycle <= 5; cycle++) {
        // 1. Read ADC (simulate voltage measurement)
        float sampled_voltage = read_adc_voltage();
        
        // 2. Calculate error
        float error = target_voltage - sampled_voltage;
        
        // 3. Update PI controller
        float pi_output = pi_update(&pi, error, dt);
        
        // 4. Calculate duty cycle (base duty + PI correction)
        // For Boost converter: Vout = Vin/(1-D), so D = 1 - Vin/Vout
        // With Vin=12V, Vout=24V, ideal D = 0.5
        float base_duty = 0.5f; // Ideal duty for 12V->24V conversion
        float duty_cycle = base_duty + pi_output;
        
        // 5. Set PWM duty cycle
        set_pwm_duty(duty_cycle);
        
        // 6. Print results
        printf("周期%d: 采样=%.1fV, 误差=%.1fV, PI输出=%.2f, 占空比=%.2f\n",
               cycle, sampled_voltage, error, pi_output, duty_cycle);
    }
    
    return 0;
}