#include <stdio.h>
#include <stdint.h>

#define PWM_DUTY_REG (*((volatile uint32_t*)0x40000000))
#define ADC_VOUT_REG (*((volatile uint32_t*)0x40000004))

static volatile uint32_t mock_adc_value = 0;
static volatile uint32_t mock_pwm_duty = 0;

#define PWM_DUTY_REG mock_pwm_duty
#define ADC_VOUT_REG mock_adc_value

void soft_start_control(int *current_voltage, int target_voltage, int delta_v_max) {
    if (*current_voltage < target_voltage) {
        int delta = target_voltage - *current_voltage;
        if (delta > delta_v_max) {
            delta = delta_v_max;
        }
        *current_voltage += delta;
        
        int duty_ratio = (*current_voltage * 1000) / target_voltage;
        if (duty_ratio > 1000) duty_ratio = 1000;
        PWM_DUTY_REG = duty_ratio;
        
        ADC_VOUT_REG = *current_voltage;
    }
}

int main() {
    int vin = 12000;
    int vtarget = 24000;
    int delta_v_max = 500;
    
    int current_voltage = 0;
    int time_ms = 0;
    int print_counter = 0;
    
    printf("DC/DC Soft Start Control Simulation\n");
    printf("Input Voltage: %d mV (%.1f V)\n", vin, vin/1000.0);
    printf("Target Voltage: %d mV (%.1f V)\n", vtarget, vtarget/1000.0);
    printf("Soft Start Time: 100ms\n");
    printf("Sampling Period: 1ms\n");
    printf("Max Voltage Change Rate: %d mV/ms (%.1f V/ms)\n\n", delta_v_max, delta_v_max/1000.0);
    
    printf("Time(ms)\tOutput Voltage(V)\n");
    printf("-----------------------------\n");
    
    while (current_voltage < vtarget) {
        soft_start_control(&current_voltage, vtarget, delta_v_max);
        time_ms++;
        print_counter++;
        
        if (print_counter >= 10) {
            printf("%d\t\t%.2f\n", time_ms, current_voltage/1000.0);
            print_counter = 0;
        }
    }
    
    if (print_counter > 0) {
        printf("%d\t\t%.2f\n", time_ms, current_voltage/1000.0);
    }
    
    printf("\nSoft start completed\n");
    printf("Final Voltage: %.2f V\n", current_voltage/1000.0);
    printf("Total Time: %d ms\n", time_ms);
    
    return 0;
}