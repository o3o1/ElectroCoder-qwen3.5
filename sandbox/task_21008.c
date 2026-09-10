#include <stdio.h>
#include <math.h>

#define MAX_FREQUENCY 50.0
#define MIN_FREQUENCY 30.0

float calculate_optimal_frequency(float load_rate) {
    if (load_rate < 0.0) load_rate = 0.0;
    if (load_rate > 1.0) load_rate = 1.0;
    
    if (load_rate < 0.3) {
        return 30.0;
    } else if (load_rate < 0.7) {
        return 30.0 + (load_rate - 0.3) * 25.0;
    } else {
        return 50.0;
    }
}

float adjust_frequency(float current_freq, float target_freq, float max_change_rate) {
    if (max_change_rate <= 0.0) {
        return current_freq;
    }
    
    float diff = target_freq - current_freq;
    
    if (fabs(diff) <= max_change_rate) {
        return target_freq;
    } else {
        if (diff > 0) {
            return current_freq + max_change_rate;
        } else {
            return current_freq - max_change_rate;
        }
    }
}

void simulate_adjustment(float load_rate, float current_freq, float max_change_rate) {
    printf("测试用例: 负载率=%.1f, 当前频率=%.1fHz, 最大变化率=%.1fHz/周期\n", 
           load_rate, current_freq, max_change_rate);
    
    float target_freq = calculate_optimal_frequency(load_rate);
    printf("  计算的目标频率: %.1fHz\n", target_freq);
    
    printf("  调整过程:\n");
    float freq = current_freq;
    int step = 0;
    
    while (fabs(freq - target_freq) > 0.01) {
        freq = adjust_frequency(freq, target_freq, max_change_rate);
        step++;
        printf("    周期%d: 频率=%.1fHz\n", step, freq);
    }
    
    printf("  最终频率: %.1fHz\n\n", freq);
}

int main() {
    printf("=== 抽油机变频调速节能控制算法模拟 ===\n\n");
    
    simulate_adjustment(0.2, 40.0, 5.0);
    simulate_adjustment(0.5, 35.0, 8.0);
    simulate_adjustment(0.8, 45.0, 10.0);
    
    return 0;
}