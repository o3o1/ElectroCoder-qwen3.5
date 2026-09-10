#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// DAC硬件模拟
#define DAC_MAX_VALUE 4095  // 12位DAC最大值
#define DAC_MIN_VALUE 0     // DAC最小值

// 模拟DAC数据寄存器
uint16_t dac_output_value = 0;

// DAC设置函数
void DAC_SetValue(uint16_t value) {
    // 限制值在有效范围内
    if (value > DAC_MAX_VALUE) {
        dac_output_value = DAC_MAX_VALUE;
    } else if (value < DAC_MIN_VALUE) {
        dac_output_value = DAC_MIN_VALUE;
    } else {
        dac_output_value = value;
    }
}

// 三角波生成函数
void generate_triangle_wave(uint16_t amplitude, uint16_t step_size, uint16_t cycles) {
    uint16_t current_value;
    uint16_t change_count = 0;
    bool increasing = true;
    
    // 限制幅值不超过DAC最大值
    if (amplitude > DAC_MAX_VALUE) {
        amplitude = DAC_MAX_VALUE;
    }
    
    for (uint16_t cycle = 0; cycle < cycles; cycle++) {
        // 从最小值开始
        current_value = DAC_MIN_VALUE;
        increasing = true;
        
        while (true) {
            // 设置DAC输出值
            DAC_SetValue(current_value);
            change_count++;
            
            // 每10次变化打印一次当前值
            if (change_count % 10 == 0) {
                printf("  Change %d: DAC output = %u\n", change_count, dac_output_value);
            }
            
            // 判断方向并更新值
            if (increasing) {
                // 递增阶段
                if (current_value + step_size > amplitude) {
                    current_value = amplitude;
                } else {
                    current_value += step_size;
                }
                
                // 检查是否达到幅值
                if (current_value >= amplitude) {
                    increasing = false;
                }
            } else {
                // 递减阶段
                if (current_value < step_size) {
                    current_value = DAC_MIN_VALUE;
                } else {
                    current_value -= step_size;
                }
                
                // 检查是否回到最小值
                if (current_value <= DAC_MIN_VALUE) {
                    break;  // 完成一个周期
                }
            }
        }
    }
    
    // 打印总变化次数
    printf("  Total changes: %u\n", change_count);
}

int main() {
    printf("=== STM32 DAC三角波生成模拟 ===\n\n");
    
    // 测试用例1
    printf("测试1: 幅值=2048, 步长=5, 周期=2\n");
    generate_triangle_wave(2048, 5, 2);
    printf("\n");
    
    // 测试用例2
    printf("测试2: 幅值=1000, 步长=2, 周期=1\n");
    generate_triangle_wave(1000, 2, 1);
    printf("\n");
    
    // 测试用例3
    printf("测试3: 幅值=5000(应限制为4095), 步长=8, 周期=1\n");
    generate_triangle_wave(5000, 8, 1);
    printf("\n");
    
    printf("=== 所有测试完成 ===\n");
    return 0;
}