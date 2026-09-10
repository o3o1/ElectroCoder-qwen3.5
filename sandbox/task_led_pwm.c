#include <stdio.h>
#include <stdlib.h>

#define KP 2.0
#define KI 0.5
#define INTEGRAL_MIN -500
#define INTEGRAL_MAX 500
#define ADJUSTMENT_MIN -100
#define ADJUSTMENT_MAX 100
#define PWM_MIN 0
#define PWM_MAX 1000

int pwm_duty = 500;
float integral = 0.0;

int pi_controller(int current_adc, int target_adc) {
    int error = target_adc - current_adc;
    
    integral += error;
    
    if (integral < INTEGRAL_MIN) {
        integral = INTEGRAL_MIN;
    } else if (integral > INTEGRAL_MAX) {
        integral = INTEGRAL_MAX;
    }
    
    float adjustment_float = KP * error + KI * integral;
    int adjustment = (int)adjustment_float;
    
    if (adjustment < ADJUSTMENT_MIN) {
        adjustment = ADJUSTMENT_MIN;
    } else if (adjustment > ADJUSTMENT_MAX) {
        adjustment = ADJUSTMENT_MAX;
    }
    
    return adjustment;
}

void update_pwm_duty(int adjustment) {
    int new_duty = pwm_duty + adjustment;
    
    if (new_duty < PWM_MIN) {
        new_duty = PWM_MIN;
    } else if (new_duty > PWM_MAX) {
        new_duty = PWM_MAX;
    }
    
    pwm_duty = new_duty;
}

int main() {
    int test_cases[] = {300, 600, 450, 700, 512};
    int target_adc = 512;
    int num_tests = sizeof(test_cases) / sizeof(test_cases[0]);
    
    for (int i = 0; i < num_tests; i++) {
        int current_adc = test_cases[i];
        int adjustment = pi_controller(current_adc, target_adc);
        update_pwm_duty(adjustment);
        
        printf("ADC:%d, Target:%d, Adjustment:%+d, PWM:%d\n", 
               current_adc, target_adc, adjustment, pwm_duty);
    }
    
    return 0;
}