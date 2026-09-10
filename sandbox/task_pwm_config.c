#include <stdio.h>
#include <stdint.h>

#define TIMx_CR1 (*((volatile uint32_t *)0x40000000))
#define TIMx_PSC (*((volatile uint32_t *)0x40000028))
#define TIMx_ARR (*((volatile uint32_t *)0x4000002C))
#define TIMx_CCR1 (*((volatile uint32_t *)0x40000034))
#define TIMx_CCER (*((volatile uint32_t *)0x40000020))

typedef struct {
    uint32_t TIM_Prescaler;
    uint32_t TIM_Period;
    uint32_t TIM_Pulse;
    uint8_t  TIM_OCPolarity;
} TIM_PWM_Config;

static TIM_PWM_Config current_config;
static uint8_t pwm_enabled = 0;

void TIM_PWM_Init(TIM_PWM_Config *config) {
    current_config = *config;
    
    TIMx_PSC = config->TIM_Prescaler;
    TIMx_ARR = config->TIM_Period;
    TIMx_CCR1 = config->TIM_Pulse;
    
    if (config->TIM_OCPolarity == 0) {
        TIMx_CCER |= (1 << 1);
    } else {
        TIMx_CCER &= ~(1 << 1);
    }
    
    printf("[INFO] PWM初始化完成：预分频=%u，周期=%u，脉冲=%u，极性=%u\n",
           config->TIM_Prescaler, config->TIM_Period, config->TIM_Pulse, config->TIM_OCPolarity);
}

void TIM_SetPulse(uint32_t pulse) {
    current_config.TIM_Pulse = pulse;
    TIMx_CCR1 = pulse;
    printf("[INFO] 脉冲宽度修改为：%u\n", pulse);
}

void TIM_EnablePWM(uint8_t enable) {
    pwm_enabled = enable;
    if (enable) {
        TIMx_CR1 |= (1 << 0);
        printf("[INFO] PWM输出已启用\n");
    } else {
        TIMx_CR1 &= ~(1 << 0);
        printf("[INFO] PWM输出已禁用\n");
    }
}

int main() {
    TIM_PWM_Config pwm_config = {
        .TIM_Prescaler = 72,
        .TIM_Period = 1000,
        .TIM_Pulse = 500,
        .TIM_OCPolarity = 0
    };
    
    TIM_PWM_Init(&pwm_config);
    TIM_EnablePWM(1);
    TIM_SetPulse(600);
    
    printf("[INFO] 当前配置：预分频=%u，周期=%u，脉冲=%u，极性=%u，使能状态=%u\n",
           current_config.TIM_Prescaler,
           current_config.TIM_Period,
           current_config.TIM_Pulse,
           current_config.TIM_OCPolarity,
           pwm_enabled);
    
    return 0;
}