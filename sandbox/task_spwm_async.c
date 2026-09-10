#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define CARRIER_RATIO 10.7f  // 载波比，非整数实现异步调制

void generateSPWM(float modulationIndex, float frequency, int samplesPerCycle, float* phaseU, float* phaseV, float* phaseW) {
    if (modulationIndex < 0.0f || modulationIndex > 1.0f) {
        modulationIndex = 0.8f;  // 默认值
    }
    
    float carrierFreq = frequency * CARRIER_RATIO;  // 载波频率
    float Ts = 1.0f / (carrierFreq * samplesPerCycle);  // 采样时间间隔
    float T = 1.0f / frequency;  // 基波周期
    
    for (int i = 0; i < samplesPerCycle; i++) {
        float t = i * Ts;  // 当前时间
        float theta = 2.0f * M_PI * frequency * t;  // 电角度
        
        // 生成三相正弦调制波（带三次谐波注入）
        float sinU = sinf(theta);
        float sinV = sinf(theta - 2.0f * M_PI / 3.0f);
        float sinW = sinf(theta + 2.0f * M_PI / 3.0f);
        
        // 三次谐波注入
        float thirdHarmonic = (1.0f / 6.0f) * sinf(3.0f * theta);
        
        // 带三次谐波注入的调制波
        float modU = modulationIndex * (sinU + thirdHarmonic);
        float modV = modulationIndex * (sinV + thirdHarmonic);
        float modW = modulationIndex * (sinW + thirdHarmonic);
        
        // 三角载波生成
        float carrierAngle = 2.0f * M_PI * carrierFreq * t;
        float carrier = fmodf(carrierAngle, 2.0f * M_PI) / (M_PI);  // 归一化到[0,2]
        
        // 三角波：0到1上升，1到2下降
        float triangleWave;
        if (carrier <= 1.0f) {
            triangleWave = carrier;  // 上升沿
        } else {
            triangleWave = 2.0f - carrier;  // 下降沿
        }
        
        // 比较调制波和载波生成PWM
        phaseU[i] = (modU > triangleWave) ? 1.0f : 0.0f;
        phaseV[i] = (modV > triangleWave) ? 1.0f : 0.0f;
        phaseW[i] = (modW > triangleWave) ? 1.0f : 0.0f;
    }
}

int main() {
    float modulationIndex = 0.8f;
    float frequency = 50.0f;
    int samplesPerCycle = 200;
    
    // 分配内存存储波形数据
    float* phaseU = (float*)malloc(samplesPerCycle * sizeof(float));
    float* phaseV = (float*)malloc(samplesPerCycle * sizeof(float));
    float* phaseW = (float*)malloc(samplesPerCycle * sizeof(float));
    
    if (!phaseU || !phaseV || !phaseW) {
        printf("内存分配失败\n");
        return 1;
    }
    
    // 生成SPWM波形
    generateSPWM(modulationIndex, frequency, samplesPerCycle, phaseU, phaseV, phaseW);
    
    // 打印前10个采样点
    printf("三相逆变器异步SPWM调制波形（带三次谐波抑制）\n");
    printf("调制比: %.2f, 基波频率: %.1f Hz, 每周期采样点数: %d\n\n", modulationIndex, frequency, samplesPerCycle);
    printf("前10个采样点数据:\n");
    for (int i = 0; i < 10 && i < samplesPerCycle; i++) {
        printf("Sample[%d]: U=%.4f, V=%.4f, W=%.4f\n", i, phaseU[i], phaseV[i], phaseW[i]);
    }
    
    // 计算三相平均值
    float sumU = 0.0f, sumV = 0.0f, sumW = 0.0f;
    for (int i = 0; i < samplesPerCycle; i++) {
        sumU += phaseU[i];
        sumV += phaseV[i];
        sumW += phaseW[i];
    }
    
    float avgU = sumU / samplesPerCycle;
    float avgV = sumV / samplesPerCycle;
    float avgW = sumW / samplesPerCycle;
    
    printf("\n三相波形平均值（用于验证对称性）:\n");
    printf("U相平均值: %.6f\n", avgU);
    printf("V相平均值: %.6f\n", avgV);
    printf("W相平均值: %.6f\n", avgW);
    
    // 释放内存
    free(phaseU);
    free(phaseV);
    free(phaseW);
    
    return 0;
}