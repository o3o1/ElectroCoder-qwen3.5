#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define PWM_OUTPUT(x) printf("[PWM] duty set to %d%%\n", x)

typedef struct {
    float current_temp;
    float target_temp;
    float kp;
    uint8_t pwm_duty;
} FanController;

void update_pwm_duty(FanController *fc) {
    float error = fc->target_temp - fc->current_temp;
    float adjustment = fc->kp * error;
    float new_duty = fc->pwm_duty + adjustment;
    
    if (new_duty < 0) {
        new_duty = 0;
    } else if (new_duty > 100) {
        new_duty = 100;
    }
    
    fc->pwm_duty = (uint8_t)roundf(new_duty);
}

int main() {
    FanController fan = {
        .current_temp = 45.0f,
        .target_temp = 40.0f,
        .kp = 2.5f,
        .pwm_duty = 50
    };
    
    printf("Initial state:\n");
    printf("Current temp: %.1f°C\n", fan.current_temp);
    printf("Target temp: %.1f°C\n", fan.target_temp);
    printf("KP: %.1f\n", fan.kp);
    printf("PWM duty: %d%%\n", fan.pwm_duty);
    printf("\n");
    
    update_pwm_duty(&fan);
    PWM_OUTPUT(fan.pwm_duty);
    
    printf("\nUpdated state (temp 45°C):\n");
    printf("Current temp: %.1f°C\n", fan.current_temp);
    printf("Target temp: %.1f°C\n", fan.target_temp);
    printf("KP: %.1f\n", fan.kp);
    printf("PWM duty: %d%%\n", fan.pwm_duty);
    printf("\n");
    
    fan.current_temp = 38.0f;
    fan.pwm_duty = 50;
    
    printf("Test case (temp 38°C):\n");
    printf("Current temp: %.1f°C\n", fan.current_temp);
    printf("Target temp: %.1f°C\n", fan.target_temp);
    printf("KP: %.1f\n", fan.kp);
    printf("PWM duty: %d%%\n", fan.pwm_duty);
    printf("\n");
    
    update_pwm_duty(&fan);
    PWM_OUTPUT(fan.pwm_duty);
    
    printf("\nUpdated state (temp 38°C):\n");
    printf("Current temp: %.1f°C\n", fan.current_temp);
    printf("Target temp: %.1f°C\n", fan.target_temp);
    printf("KP: %.1f\n", fan.kp);
    printf("PWM duty: %d%%\n", fan.pwm_duty);
    
    return 0;
}