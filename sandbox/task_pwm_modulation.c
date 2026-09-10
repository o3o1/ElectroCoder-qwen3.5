#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define REFERENCE_VOLTAGE 12.0f

typedef struct {
    uint32_t period_ticks;
    uint32_t compare_ticks;
    float modulation_depth;
    float output_voltage;
} PWM_Controller;

void PWM_Init(PWM_Controller *pwm, uint32_t period, uint32_t compare, float depth) {
    pwm->period_ticks = period;
    pwm->compare_ticks = compare;
    pwm->modulation_depth = depth;
    pwm->output_voltage = 0.0f;
}

void PWM_SetModulationDepth(PWM_Controller *pwm, float depth) {
    if (depth < 0.0f) {
        pwm->modulation_depth = 0.0f;
    } else if (depth > 1.0f) {
        pwm->modulation_depth = 1.0f;
    } else {
        pwm->modulation_depth = depth;
    }
}

void PWM_UpdateOutput(PWM_Controller *pwm) {
    float duty_cycle = (float)pwm->compare_ticks / (float)pwm->period_ticks;
    pwm->output_voltage = duty_cycle * pwm->modulation_depth * REFERENCE_VOLTAGE;
}

float PWM_GetOutputVoltage(PWM_Controller *pwm) {
    return pwm->output_voltage;
}

int main() {
    PWM_Controller pwm1, pwm2;
    
    PWM_Init(&pwm1, 1000, 300, 0.3f);
    PWM_UpdateOutput(&pwm1);
    printf("PWM1: period=%u, compare=%u, depth=%.2f -> output=%.2fV\n",
           pwm1.period_ticks, pwm1.compare_ticks, pwm1.modulation_depth, pwm1.output_voltage);
    
    PWM_Init(&pwm2, 1000, 700, 0.7f);
    PWM_UpdateOutput(&pwm2);
    printf("PWM2: period=%u, compare=%u, depth=%.2f -> output=%.2fV\n",
           pwm2.period_ticks, pwm2.compare_ticks, pwm2.modulation_depth, pwm2.output_voltage);
    
    PWM_SetModulationDepth(&pwm1, 1.0f);
    PWM_UpdateOutput(&pwm1);
    printf("PWM1: period=%u, compare=%u, depth=%.2f -> output=%.2fV\n",
           pwm1.period_ticks, pwm1.compare_ticks, pwm1.modulation_depth, pwm1.output_voltage);
    
    PWM_SetModulationDepth(&pwm2, 1.0f);
    PWM_UpdateOutput(&pwm2);
    printf("PWM2: period=%u, compare=%u, depth=%.2f -> output=%.2fV\n",
           pwm2.period_ticks, pwm2.compare_ticks, pwm2.modulation_depth, pwm2.output_voltage);
    
    PWM_SetModulationDepth(&pwm1, 0.5f);
    PWM_UpdateOutput(&pwm1);
    printf("PWM1: period=%u, compare=%u, depth=%.2f -> output=%.2fV\n",
           pwm1.period_ticks, pwm1.compare_ticks, pwm1.modulation_depth, pwm1.output_voltage);
    
    PWM_SetModulationDepth(&pwm2, 0.3f);
    PWM_UpdateOutput(&pwm2);
    printf("PWM2: period=%u, compare=%u, depth=%.2f -> output=%.2fV\n",
           pwm2.period_ticks, pwm2.compare_ticks, pwm2.modulation_depth, pwm2.output_voltage);
    
    return 0;
}