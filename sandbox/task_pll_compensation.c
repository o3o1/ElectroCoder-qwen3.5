#include <stdio.h>
#include <math.h>

#define PI 3.14159265358979323846
#define TWO_PI (2.0 * PI)

double compensate_phase_delay(double theta_current, double freq, int delay_cycles, double T) {
    double phase_increment = TWO_PI * freq * delay_cycles * T;
    double theta_compensated = theta_current + phase_increment;
    
    while (theta_compensated >= TWO_PI) {
        theta_compensated -= TWO_PI;
    }
    
    while (theta_compensated < 0.0) {
        theta_compensated += TWO_PI;
    }
    
    return theta_compensated;
}

int main() {
    double T = 0.0001;
    
    printf("PLL死区时间补偿算法测试\n");
    printf("控制周期 T = %f 秒 (10kHz)\n\n", T);
    
    printf("测试用例1: 常规情况（补偿后仍在[0,2π)内）\n");
    double theta1 = 1.0;
    double freq1 = 50.0;
    int delay1 = 2;
    double compensated1 = compensate_phase_delay(theta1, freq1, delay1, T);
    printf("当前相位: %.6f rad, 频率: %.2f Hz, 延迟周期: %d\n", theta1, freq1, delay1);
    printf("补偿后相位: %.6f rad\n\n", compensated1);
    
    printf("测试用例2: 补偿后超过2π的情况\n");
    double theta2 = 5.5;
    double freq2 = 50.0;
    int delay2 = 5;
    double compensated2 = compensate_phase_delay(theta2, freq2, delay2, T);
    printf("当前相位: %.6f rad, 频率: %.2f Hz, 延迟周期: %d\n", theta2, freq2, delay2);
    printf("补偿后相位: %.6f rad\n\n", compensated2);
    
    printf("测试用例3: 补偿后为负值的情况（频率为负）\n");
    double theta3 = 1.0;
    double freq3 = -50.0;
    int delay3 = 3;
    double compensated3 = compensate_phase_delay(theta3, freq3, delay3, T);
    printf("当前相位: %.6f rad, 频率: %.2f Hz, 延迟周期: %d\n", theta3, freq3, delay3);
    printf("补偿后相位: %.6f rad\n\n", compensated3);
    
    printf("测试用例4: 边界情况测试\n");
    double theta4 = 6.28;
    double freq4 = 50.0;
    int delay4 = 1;
    double compensated4 = compensate_phase_delay(theta4, freq4, delay4, T);
    printf("当前相位: %.6f rad (接近2π), 频率: %.2f Hz, 延迟周期: %d\n", theta4, freq4, delay4);
    printf("补偿后相位: %.6f rad\n", compensated4);
    
    return 0;
}