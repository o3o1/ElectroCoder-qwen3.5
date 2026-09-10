#include <stdio.h>
#include <math.h>
#include <float.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define CARRIER_FREQUENCY 10000.0f  // 10kHz固定载波频率

float calculateSPWMDutyCycle(float time, float frequency, float modulationIndex) {
    // 计算正弦调制波值
    float sine_value = sinf(2.0f * (float)M_PI * frequency * time);
    
    // 计算占空比：duty = 0.5 + 0.5 * modulationIndex * sin(2πft)
    float duty = 0.5f + 0.5f * modulationIndex * sine_value;
    
    // 限制在0.0到1.0范围内
    if (duty < 0.0f) {
        duty = 0.0f;
    } else if (duty > 1.0f) {
        duty = 1.0f;
    }
    
    return duty;
}

int main() {
    // 定义测试用例数组
    struct TestCase {
        float frequency;
        float modulationIndex;
        float time;
    } testCases[] = {
        {50.0f, 0.8f, 0.005f},      // 正常情况：50Hz, 调制比0.8, 时间5ms
        {60.0f, 0.9f, 0.002f},      // 正常情况：60Hz, 调制比0.9, 时间2ms
        {0.0f, 1.2f, 0.001f},       // 边界条件：频率0Hz, 调制比1.2(超调)
        {50.0f, 0.0f, 0.01f},       // 边界条件：调制比为0
        {50.0f, 1.0f, 0.0f},        // 边界条件：时间为0
        {100.0f, 0.5f, 0.001f},     // 高频情况
        {25.0f, 1.1f, 0.0025f},     // 调制比超调测试
    };
    
    int numCases = sizeof(testCases) / sizeof(testCases[0]);
    
    printf("异步调制SPWM占空比计算测试（载波频率固定为%.0f Hz）\n", CARRIER_FREQUENCY);
    printf("====================================================\n");
    
    for (int i = 0; i < numCases; i++) {
        float duty = calculateSPWMDutyCycle(
            testCases[i].time,
            testCases[i].frequency,
            testCases[i].modulationIndex
        );
        
        printf("频率=%.1f Hz, 调制比=%.2f, 时间=%.4f s => 占空比=%.6f\n",
               testCases[i].frequency,
               testCases[i].modulationIndex,
               testCases[i].time,
               duty);
    }
    
    // 额外测试：验证正弦波周期特性
    printf("\n正弦波周期特性验证（50Hz, 调制比0.8）：\n");
    float freq = 50.0f;
    float mi = 0.8f;
    float period = 1.0f / freq;
    
    for (int i = 0; i < 5; i++) {
        float t = i * period / 4.0f;  // 0°, 90°, 180°, 270°, 360°
        float duty = calculateSPWMDutyCycle(t, freq, mi);
        printf("  时间=%.4f s (相位=%d°) => 占空比=%.6f\n", t, i * 90, duty);
    }
    
    return 0;
}