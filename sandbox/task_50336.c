#include <stdio.h>
#include <math.h>

#define PI 3.141592653589793f

float pll_phase_compensation(float measured_phase, float filter_delay, 
                            float system_freq, float sampling_time, 
                            int compensation_mode) {
    float compensated_phase = measured_phase;
    
    switch(compensation_mode) {
        case 0: // 不补偿
            break;
            
        case 1: // 线性补偿
            compensated_phase = measured_phase + 2.0f * PI * system_freq * filter_delay;
            break;
            
        case 2: // 预测补偿
            compensated_phase = measured_phase + 2.0f * PI * system_freq * (filter_delay + sampling_time);
            break;
            
        default:
            break;
    }
    
    return compensated_phase;
}

int main() {
    // 测试用例a
    printf("测试1: 频率=50Hz, 测量相位=0.785rad, 延迟=0.002s, 采样=0.0001s\n");
    float result0 = pll_phase_compensation(0.785f, 0.002f, 50.0f, 0.0001f, 0);
    float result1 = pll_phase_compensation(0.785f, 0.002f, 50.0f, 0.0001f, 1);
    float result2 = pll_phase_compensation(0.785f, 0.002f, 50.0f, 0.0001f, 2);
    printf("  不补偿: %.6f rad\n", result0);
    printf("  线性补偿: %.6f rad\n", result1);
    printf("  预测补偿: %.6f rad\n", result2);
    printf("\n");
    
    // 测试用例b
    printf("测试2: 频率=60Hz, 测量相位=1.047rad, 延迟=0.0015s, 采样=0.00005s\n");
    result0 = pll_phase_compensation(1.047f, 0.0015f, 60.0f, 0.00005f, 0);
    result1 = pll_phase_compensation(1.047f, 0.0015f, 60.0f, 0.00005f, 1);
    result2 = pll_phase_compensation(1.047f, 0.0015f, 60.0f, 0.00005f, 2);
    printf("  不补偿: %.6f rad\n", result0);
    printf("  线性补偿: %.6f rad\n", result1);
    printf("  预测补偿: %.6f rad\n", result2);
    printf("\n");
    
    // 测试用例c - 自定义参数
    printf("测试3: 频率=400Hz, 测量相位=0.523rad, 延迟=0.0005s, 采样=0.000025s\n");
    result0 = pll_phase_compensation(0.523f, 0.0005f, 400.0f, 0.000025f, 0);
    result1 = pll_phase_compensation(0.523f, 0.0005f, 400.0f, 0.000025f, 1);
    result2 = pll_phase_compensation(0.523f, 0.0005f, 400.0f, 0.000025f, 2);
    printf("  不补偿: %.6f rad\n", result0);
    printf("  线性补偿: %.6f rad\n", result1);
    printf("  预测补偿: %.6f rad\n", result2);
    
    return 0;
}