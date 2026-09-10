#include <stdio.h>
#include <stdint.h>

#define PWM_PERIOD 1000
#define DEAD_TIME 50
volatile uint32_t *PWM1_H, *PWM1_L, *PWM2_H, *PWM2_L;

void phase_shift_pwm(uint32_t duty1, uint32_t duty2, uint32_t phase_shift) {
    if (duty1 > PWM_PERIOD) duty1 = PWM_PERIOD;
    if (duty2 > PWM_PERIOD) duty2 = PWM_PERIOD;
    if (phase_shift > PWM_PERIOD) phase_shift = phase_shift % PWM_PERIOD;
    
    *PWM1_H = duty1;
    *PWM1_L = (duty1 + DEAD_TIME) % PWM_PERIOD;
    *PWM2_H = (duty2 + phase_shift) % PWM_PERIOD;
    *PWM2_L = (duty2 + phase_shift + DEAD_TIME) % PWM_PERIOD;
}

int main() {
    uint32_t reg1 = 0, reg2 = 0, reg3 = 0, reg4 = 0;
    
    PWM1_H = &reg1;
    PWM1_L = &reg2;
    PWM2_H = &reg3;
    PWM2_L = &reg4;
    
    phase_shift_pwm(600, 600, 200);
    printf("Test 1: PWM1_H=%u, PWM1_L=%u, PWM2_H=%u, PWM2_L=%u\n", reg1, reg2, reg3, reg4);
    
    phase_shift_pwm(300, 700, 500);
    printf("Test 2: PWM1_H=%u, PWM1_L=%u, PWM2_H=%u, PWM2_L=%u\n", reg1, reg2, reg3, reg4);
    
    phase_shift_pwm(900, 400, 800);
    printf("Test 3: PWM1_H=%u, PWM1_L=%u, PWM2_H=%u, PWM2_L=%u\n", reg1, reg2, reg3, reg4);
    
    return 0;
}