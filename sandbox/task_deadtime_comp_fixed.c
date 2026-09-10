#include <stdio.h>

float dead_time_compensation(float duty_cycle, float dead_time_ns, float switching_period_ns, int is_high_side) {
    float compensation = dead_time_ns / switching_period_ns;
    float compensated_duty;
    
    if (is_high_side) {
        compensated_duty = duty_cycle + compensation;
    } else {
        compensated_duty = duty_cycle - compensation;
    }
    
    if (compensated_duty > 1.0f) {
        compensated_duty = 1.0f;
    } else if (compensated_duty < 0.0f) {
        compensated_duty = 0.0f;
    }
    
    return compensated_duty;
}

int main() {
    printf("Dead-time Compensation Algorithm Test\n");
    printf("=====================================\n");
    
    float dead_time_ns = 2000.0f;
    float switching_period_ns = 100000.0f;
    
    float test1_duty = 0.3f;
    float test1_comp = dead_time_compensation(test1_duty, dead_time_ns, switching_period_ns, 1);
    printf("Test 1 - High-side compensation:\n");
    printf("  Original duty cycle: %.4f\n", test1_duty);
    printf("  Compensated duty cycle: %.4f\n", test1_comp);
    printf("  Compensation amount: +%.4f\n", dead_time_ns / switching_period_ns);
    printf("\n");
    
    float test2_duty = 0.8f;
    float test2_comp = dead_time_compensation(test2_duty, dead_time_ns, switching_period_ns, 0);
    printf("Test 2 - Low-side compensation:\n");
    printf("  Original duty cycle: %.4f\n", test2_duty);
    printf("  Compensated duty cycle: %.4f\n", test2_comp);
    printf("  Compensation amount: -%.4f\n", dead_time_ns / switching_period_ns);
    printf("\n");
    
    float test3_duty = 0.01f;
    float test3_comp = dead_time_compensation(test3_duty, dead_time_ns, switching_period_ns, 0);
    printf("Test 3 - Boundary test (Low-side):\n");
    printf("  Original duty cycle: %.4f\n", test3_duty);
    printf("  Compensated duty cycle: %.4f\n", test3_comp);
    printf("  Compensation amount: -%.4f\n", dead_time_ns / switching_period_ns);
    printf("\n");
    
    float test4_duty = 0.99f;
    float test4_comp = dead_time_compensation(test4_duty, dead_time_ns, switching_period_ns, 1);
    printf("Test 4 - Boundary test (High-side):\n");
    printf("  Original duty cycle: %.4f\n", test4_duty);
    printf("  Compensated duty cycle: %.4f\n", test4_comp);
    printf("  Compensation amount: +%.4f\n", dead_time_ns / switching_period_ns);
    
    return 0;
}