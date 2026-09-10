#include <stdio.h>

double pwm_limit(double duty, double min_limit, double max_limit) {
    if (duty < min_limit) {
        return min_limit;
    } else if (duty > max_limit) {
        return max_limit;
    } else {
        return duty;
    }
}

int main() {
    // 测试用例1: 低于下限的情况
    double duty1 = 0.02;
    double min1 = 0.05;
    double max1 = 0.95;
    double result1 = pwm_limit(duty1, min1, max1);
    printf("原始占空比:%.2f, 下限:%.2f, 上限:%.2f => 限幅后:%.2f\n", duty1, min1, max1, result1);
    
    // 测试用例2: 正常范围内的情况
    double duty2 = 0.6;
    double min2 = 0.05;
    double max2 = 0.95;
    double result2 = pwm_limit(duty2, min2, max2);
    printf("原始占空比:%.2f, 下限:%.2f, 上限:%.2f => 限幅后:%.2f\n", duty2, min2, max2, result2);
    
    // 测试用例3: 高于上限的情况
    double duty3 = 0.98;
    double min3 = 0.05;
    double max3 = 0.95;
    double result3 = pwm_limit(duty3, min3, max3);
    printf("原始占空比:%.2f, 下限:%.2f, 上限:%.2f => 限幅后:%.2f\n", duty3, min3, max3, result3);
    
    return 0;
}