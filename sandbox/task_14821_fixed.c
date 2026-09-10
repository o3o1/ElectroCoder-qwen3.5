#include <stdio.h>
#include <stdint.h>

#define PWM_COMPARE_REG (*((volatile uint32_t*)0x40000000))
#define PWM_MAX_VALUE 3000
#define BASE_FREQ 30.0f
#define MAX_FREQ 55.0f

float calculate_frequency(float load_ratio) {
    if (load_ratio < 0.3f) {
        return BASE_FREQ;
    } else if (load_ratio <= 0.7f) {
        return BASE_FREQ + (load_ratio - 0.3f) * 25.0f;
    } else {
        return MAX_FREQ;
    }
}

void adjust_frequency(float target_freq) {
    printf("调节PWM占空比...\n");
    
    float freq_ratio = target_freq / MAX_FREQ;
    uint32_t target_pwm = (uint32_t)(freq_ratio * PWM_MAX_VALUE);
    
    uint32_t current_pwm = PWM_COMPARE_REG;
    
    if (current_pwm == 0) {
        current_pwm = 1000;
    }
    
    while (current_pwm != target_pwm) {
        if (current_pwm < target_pwm) {
            current_pwm += 100;
            if (current_pwm > target_pwm) {
                current_pwm = target_pwm;
            }
        } else {
            current_pwm -= 100;
            if (current_pwm < target_pwm) {
                current_pwm = target_pwm;
            }
        }
        
        PWM_COMPARE_REG = current_pwm;
        printf("当前值: %u\n", current_pwm);
    }
    
    printf("频率稳定在%.2f Hz\n", target_freq);
}

int main() {
    float test_loads[] = {0.15f, 0.45f, 0.85f};
    int num_tests = sizeof(test_loads) / sizeof(test_loads[0]);
    
    for (int i = 0; i < num_tests; i++) {
        float load = test_loads[i];
        float freq = calculate_frequency(load);
        
        printf("\n负载率: %.2f -> 目标频率: %.2f Hz\n", load, freq);
        adjust_frequency(freq);
    }
    
    return 0;
}