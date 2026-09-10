#include <stdio.h>
#include <stdbool.h>

#define GRID_FREQ_NOMINAL 50.0
#define FREQ_UPPER_LIMIT 50.5
#define FREQ_LOWER_LIMIT 49.5
#define AFD_PERTURBATION 0.02
#define MAX_CYCLES 10

float detect_islanding(float measured_freq) {
    float perturbed_freq;
    
    if (measured_freq > GRID_FREQ_NOMINAL) {
        perturbed_freq = measured_freq - AFD_PERTURBATION;
    } else {
        perturbed_freq = measured_freq + AFD_PERTURBATION;
    }
    
    return perturbed_freq;
}

void test_grid_normal() {
    printf("=== 测试1：电网正常情况 ===\n");
    float current_freq = 50.0;
    
    for (int i = 0; i < MAX_CYCLES; i++) {
        float perturbed = detect_islanding(current_freq);
        printf("周期 %d: 测量频率=%.3fHz, 扰动后频率=%.3fHz\n", 
               i+1, current_freq, perturbed);
        current_freq = 50.0;
    }
    printf("电网正常：频率稳定在额定值附近\n\n");
}

void test_islanding_scenario() {
    printf("=== 测试2：孤岛发生情况 ===\n");
    float current_freq = 50.0;
    bool island_detected = false;
    
    for (int cycle = 0; cycle < MAX_CYCLES; cycle++) {
        float perturbed = detect_islanding(current_freq);
        
        printf("周期 %d: 测量频率=%.3fHz, 扰动后频率=%.3fHz", 
               cycle+1, current_freq, perturbed);
        
        if (perturbed > FREQ_UPPER_LIMIT || perturbed < FREQ_LOWER_LIMIT) {
            printf(" -> 超出阈值！\n");
            island_detected = true;
            break;
        } else {
            printf(" -> 在正常范围内\n");
        }
        
        current_freq += 0.1;
    }
    
    if (island_detected) {
        printf("结果：孤岛检测触发\n");
    } else {
        printf("结果：未触发（达到最大检测周期）\n");
    }
}

int main() {
    test_grid_normal();
    test_islanding_scenario();
    return 0;
}