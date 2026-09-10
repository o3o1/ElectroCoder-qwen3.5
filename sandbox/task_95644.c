#include <stdio.h>

#define SWITCHING_PERIOD_US 50.0f
#define DEAD_TIME_US 2.0f

float dead_time_compensation(float duty_cycle, float dead_time_us, float switching_period_us) {
    if (duty_cycle > 0.5f) {
        return duty_cycle + dead_time_us / switching_period_us;
    } else if (duty_cycle < 0.5f) {
        return duty_cycle - dead_time_us / switching_period_us;
    } else {
        return duty_cycle;
    }
}

int main() {
    float test_cases[] = {0.6f, 0.4f, 0.5f};
    int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);
    
    printf("开关周期: %.1f us, 死区时间: %.1f us\n", SWITCHING_PERIOD_US, DEAD_TIME_US);
    printf("========================================\n");
    
    for (int i = 0; i < num_cases; i++) {
        float original_duty = test_cases[i];
        float compensated_duty = dead_time_compensation(original_duty, DEAD_TIME_US, SWITCHING_PERIOD_US);
        
        printf("原始占空比: %.3f, 补偿后: %.3f\n", original_duty, compensated_duty);
    }
    
    return 0;
}