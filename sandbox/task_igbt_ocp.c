#include <stdio.h>
#include <unistd.h>

#define CURRENT_THRESHOLD 100.0f
#define PWM_REGISTER (*((volatile unsigned int*)0x40000000))

volatile unsigned int mock_pwm_register = 100;

float read_collector_current(void) {
    static int call_count = 0;
    float currents[] = {80.0f, 120.0f, 99.9f};
    float current = currents[call_count % 3];
    call_count++;
    return current;
}

void soft_shutdown(void) {
    printf("Overcurrent detected, soft shutdown initiated\n");
    
    for (int duty_cycle = 100; duty_cycle >= 0; duty_cycle -= 10) {
        mock_pwm_register = duty_cycle;
        printf("PWM duty cycle: %d%%\n", duty_cycle);
        usleep(100000);
    }
    
    mock_pwm_register = 0;
    printf("Soft shutdown completed\n");
}

int main(void) {
    printf("IGBT Overcurrent Protection Simulation\n");
    printf("======================================\n\n");
    
    for (int test_case = 0; test_case < 3; test_case++) {
        printf("Test Case %d:\n", test_case + 1);
        
        float current = read_collector_current();
        printf("Measured current: %.1fA\n", current);
        
        if (current >= CURRENT_THRESHOLD) {
            soft_shutdown();
        } else if (current >= CURRENT_THRESHOLD * 0.95f) {
            printf("Current near threshold\n");
        } else {
            printf("Current normal\n");
        }
        
        printf("\n");
    }
    
    return 0;
}