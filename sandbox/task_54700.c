#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define RATED_CAPACITY 3000.0  // mAh
#define RATED_RESISTANCE 50.0  // mΩ

float calculate_soh(float current_capacity, float current_resistance, 
                    float rated_capacity, float rated_resistance, float w1) {
    if (w1 < 0.0f) w1 = 0.0f;
    if (w1 > 1.0f) w1 = 1.0f;
    
    float w2 = 1.0f - w1;
    
    float soh_capacity = (current_capacity / rated_capacity) * 100.0f;
    if (soh_capacity > 100.0f) soh_capacity = 100.0f;
    if (soh_capacity < 0.0f) soh_capacity = 0.0f;
    
    float soh_resistance = (rated_resistance / current_resistance) * 100.0f;
    if (soh_resistance > 100.0f) soh_resistance = 100.0f;
    if (soh_resistance < 0.0f) soh_resistance = 0.0f;
    
    float combined_soh = w1 * soh_capacity + w2 * soh_resistance;
    if (combined_soh > 100.0f) combined_soh = 100.0f;
    if (combined_soh < 0.0f) combined_soh = 0.0f;
    
    return combined_soh;
}

int main() {
    printf("锂电池SOH估算算法测试\n");
    printf("额定容量: %.1f mAh, 额定内阻: %.1f mΩ\n\n", RATED_CAPACITY, RATED_RESISTANCE);
    
    // 测试用例a: 正常衰减
    float current_capacity_a = 2800.0f;
    float current_resistance_a = 60.0f;
    float w1_a = 0.7f;
    
    float soh_capacity_a = (current_capacity_a / RATED_CAPACITY) * 100.0f;
    float soh_resistance_a = (RATED_RESISTANCE / current_resistance_a) * 100.0f;
    float combined_soh_a = calculate_soh(current_capacity_a, current_resistance_a, 
                                        RATED_CAPACITY, RATED_RESISTANCE, w1_a);
    
    printf("测试a: 容量SOH=%.2f%% 内阻SOH=%.2f%% 综合SOH=%.2f%%\n", 
           soh_capacity_a, soh_resistance_a, combined_soh_a);
    
    // 测试用例b: 严重衰减
    float current_capacity_b = 2000.0f;
    float current_resistance_b = 100.0f;
    float w1_b = 0.7f;
    
    float soh_capacity_b = (current_capacity_b / RATED_CAPACITY) * 100.0f;
    float soh_resistance_b = (RATED_RESISTANCE / current_resistance_b) * 100.0f;
    float combined_soh_b = calculate_soh(current_capacity_b, current_resistance_b, 
                                        RATED_CAPACITY, RATED_RESISTANCE, w1_b);
    
    printf("测试b: 容量SOH=%.2f%% 内阻SOH=%.2f%% 综合SOH=%.2f%%\n", 
           soh_capacity_b, soh_resistance_b, combined_soh_b);
    
    // 测试用例c: 内阻主导衰减
    float current_capacity_c = 2900.0f;
    float current_resistance_c = 150.0f;
    float w1_c = 0.3f;
    
    float soh_capacity_c = (current_capacity_c / RATED_CAPACITY) * 100.0f;
    float soh_resistance_c = (RATED_RESISTANCE / current_resistance_c) * 100.0f;
    float combined_soh_c = calculate_soh(current_capacity_c, current_resistance_c, 
                                        RATED_CAPACITY, RATED_RESISTANCE, w1_c);
    
    printf("测试c: 容量SOH=%.2f%% 内阻SOH=%.2f%% 综合SOH=%.2f%%\n", 
           soh_capacity_c, soh_resistance_c, combined_soh_c);
    
    return 0;
}