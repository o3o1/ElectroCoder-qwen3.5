#include <stdio.h>
#include <stdint.h>

// 模拟DAC数据寄存器
#define DAC_DHR12R1 (*((volatile uint32_t*)0x40007408))

// 三角波生成函数
void generate_triangle_wave(uint16_t amplitude, uint16_t step, uint16_t cycles) {
    uint16_t dac_value = 0;
    int increasing = 1;
    uint32_t point_count = 0;
    
    for (uint16_t cycle = 0; cycle < cycles; cycle++) {
        // 递增阶段
        increasing = 1;
        while (increasing) {
            // 模拟DAC输出
            DAC_DHR12R1 = dac_value;
            printf("DAC输出: %u", dac_value);
            point_count++;
            
            // 每10个点换行
            if (point_count % 10 == 0) {
                printf("\n");
            } else {
                printf(" ");
            }
            
            // 递增
            if (dac_value + step > amplitude) {
                dac_value = amplitude;
                increasing = 0;
            } else {
                dac_value += step;
            }
            
            // 如果已经达到峰值，切换到递减
            if (dac_value >= amplitude) {
                increasing = 0;
            }
        }
        
        // 递减阶段
        while (!increasing && dac_value > 0) {
            // 模拟DAC输出
            DAC_DHR12R1 = dac_value;
            printf("DAC输出: %u", dac_value);
            point_count++;
            
            // 每10个点换行
            if (point_count % 10 == 0) {
                printf("\n");
            } else {
                printf(" ");
            }
            
            // 递减
            if (dac_value < step) {
                dac_value = 0;
            } else {
                dac_value -= step;
            }
        }
        
        // 输出最后一个0点（如果还没输出）
        if (dac_value == 0 && cycle < cycles - 1) {
            DAC_DHR12R1 = dac_value;
            printf("DAC输出: %u", dac_value);
            point_count++;
            
            if (point_count % 10 == 0) {
                printf("\n");
            } else {
                printf(" ");
            }
        }
    }
    
    // 确保最后换行
    if (point_count % 10 != 0) {
        printf("\n");
    }
}

int main() {
    // 测试用例1
    printf("=== 测试参数: amplitude=1000, step=10, cycles=2 ===\n");
    generate_triangle_wave(1000, 10, 2);
    printf("\n");
    
    // 测试用例2
    printf("=== 测试参数: amplitude=200, step=5, cycles=3 ===\n");
    generate_triangle_wave(200, 5, 3);
    
    return 0;
}