#include <stdint.h>
#include <stdio.h>

#define TRIGGER_RISING  0
#define TRIGGER_FALLING 1
#define TRIGGER_BOTH    2

typedef struct {
    uint32_t IDR;  // 输入数据寄存器
    uint32_t CRL;  // 配置寄存器低
    uint32_t IMR;  // 中断屏蔽寄存器
    uint32_t PR;   // 中断挂起寄存器
} GPIO_TypeDef;

static uint32_t last_level = 0;
static uint32_t debounce_count = 0;
static uint32_t debounce_target = 0;
static uint32_t trigger_edge = 0;

void GPIO_Init(GPIO_TypeDef* GPIOx, uint32_t trigger_edge_config) {
    GPIOx->IDR = 0;
    GPIOx->CRL = trigger_edge_config;
    GPIOx->IMR = 1;  // 使能中断
    GPIOx->PR = 0;
    trigger_edge = trigger_edge_config;
    last_level = 0;
    debounce_count = 0;
    debounce_target = 0;
}

void GPIO_InterruptHandler(GPIO_TypeDef* GPIOx, uint32_t current_level) {
    GPIOx->IDR = current_level;
    
    uint32_t edge_detected = 0;
    uint32_t rising_edge = (last_level == 0 && current_level == 1);
    uint32_t falling_edge = (last_level == 1 && current_level == 0);
    
    if (trigger_edge == TRIGGER_RISING && rising_edge) {
        edge_detected = 1;
    } else if (trigger_edge == TRIGGER_FALLING && falling_edge) {
        edge_detected = 1;
    } else if (trigger_edge == TRIGGER_BOTH && (rising_edge || falling_edge)) {
        edge_detected = 1;
    }
    
    if (edge_detected) {
        if (debounce_count == 0) {
            debounce_target = current_level;
            debounce_count = 1;
            printf("采样值:%u | PR:%u | 状态:防抖计数中(1/3)\n", current_level, GPIOx->PR);
        } else if (current_level == debounce_target) {
            debounce_count++;
            if (debounce_count < 3) {
                printf("采样值:%u | PR:%u | 状态:防抖计数中(%u/3)\n", current_level, GPIOx->PR, debounce_count);
            } else {
                GPIOx->PR = 1;
                if (rising_edge) {
                    printf("采样值:%u | PR:%u | 状态:↑上升沿中断触发\n", current_level, GPIOx->PR);
                } else {
                    printf("采样值:%u | PR:%u | 状态:↓下降沿中断触发\n", current_level, GPIOx->PR);
                }
                debounce_count = 0;
            }
        } else {
            debounce_count = 0;
            printf("采样值:%u | PR:%u | 状态:防抖失败，重新检测\n", current_level, GPIOx->PR);
        }
    } else {
        if (debounce_count > 0 && current_level != debounce_target) {
            debounce_count = 0;
            printf("采样值:%u | PR:%u | 状态:防抖中断，重新检测\n", current_level, GPIOx->PR);
        } else if (debounce_count > 0) {
            printf("采样值:%u | PR:%u | 状态:电平稳定，继续防抖\n", current_level, GPIOx->PR);
        } else {
            printf("采样值:%u | PR:%u | 状态:无边沿变化\n", current_level, GPIOx->PR);
        }
    }
    
    last_level = current_level;
}

int main() {
    GPIO_TypeDef GPIOA;
    
    GPIO_Init(&GPIOA, TRIGGER_RISING);
    
    uint32_t test_sequence[] = {0, 0, 1, 1, 1, 0, 1, 0, 0, 1};
    uint32_t sequence_length = sizeof(test_sequence) / sizeof(test_sequence[0]);
    
    printf("GPIO中断防抖模拟测试（上升沿触发）\n");
    printf("==================================\n");
    
    for (uint32_t i = 0; i < sequence_length; i++) {
        GPIO_InterruptHandler(&GPIOA, test_sequence[i]);
    }
    
    printf("==================================\n");
    printf("测试完成\n");
    
    return 0;
}