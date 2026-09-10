#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define PI 3.14159265358979323846
#define SAMPLE_RATE 2000.0  // 2kHz采样率
#define SAMPLE_POINTS 200   // 采样点数
#define BASE_FREQ 50.0      // 基波频率50Hz
#define HARMONIC_5 250.0    // 5次谐波250Hz
#define HARMONIC_7 350.0    // 7次谐波350Hz

// 谐波分析结果结构体
typedef struct {
    double fundamental;  // 基波幅值
    double harmonic5;    // 5次谐波幅值
    double harmonic7;    // 7次谐波幅值
} HarmonicAnalysis;

// 补偿计算结果结构体
typedef struct {
    double comp_current5;  // 5次谐波补偿电流
    double comp_current7;  // 7次谐波补偿电流
} CompensationResult;

// 谐波分析函数 - 使用简化FFT方法
HarmonicAnalysis analyze_harmonics(double* samples, int num_samples) {
    HarmonicAnalysis result = {0};
    
    // 计算基波幅值
    double sum_real = 0, sum_imag = 0;
    for (int n = 0; n < num_samples; n++) {
        double angle = 2 * PI * BASE_FREQ * n / SAMPLE_RATE;
        sum_real += samples[n] * cos(angle);
        sum_imag += samples[n] * sin(angle);
    }
    result.fundamental = 2 * sqrt(sum_real*sum_real + sum_imag*sum_imag) / num_samples;
    
    // 计算5次谐波幅值
    sum_real = 0; sum_imag = 0;
    for (int n = 0; n < num_samples; n++) {
        double angle = 2 * PI * HARMONIC_5 * n / SAMPLE_RATE;
        sum_real += samples[n] * cos(angle);
        sum_imag += samples[n] * sin(angle);
    }
    result.harmonic5 = 2 * sqrt(sum_real*sum_real + sum_imag*sum_imag) / num_samples;
    
    // 计算7次谐波幅值
    sum_real = 0; sum_imag = 0;
    for (int n = 0; n < num_samples; n++) {
        double angle = 2 * PI * HARMONIC_7 * n / SAMPLE_RATE;
        sum_real += samples[n] * cos(angle);
        sum_imag += samples[n] * sin(angle);
    }
    result.harmonic7 = 2 * sqrt(sum_real*sum_real + sum_imag*sum_imag) / num_samples;
    
    return result;
}

// 有源补偿计算函数
CompensationResult calculate_compensation(HarmonicAnalysis analysis) {
    CompensationResult result = {0};
    double Kp = 1.2;  // 比例系数
    double target5 = 0.5;  // 5次谐波目标幅值
    double target7 = 0.3;  // 7次谐波目标幅值
    
    // 补偿电流 = Kp * (检测谐波 - 目标阈值)
    if (analysis.harmonic5 > target5) {
        result.comp_current5 = Kp * (analysis.harmonic5 - target5);
    }
    
    if (analysis.harmonic7 > target7) {
        result.comp_current7 = Kp * (analysis.harmonic7 - target7);
    }
    
    return result;
}

// 生成模拟信号
void generate_signal(double* samples, int num_samples) {
    for (int i = 0; i < num_samples; i++) {
        double t = i / SAMPLE_RATE;
        // 基波: 10A @ 50Hz
        // 5次谐波: 2A @ 250Hz
        // 7次谐波: 1.5A @ 350Hz
        samples[i] = 10.0 * sin(2 * PI * BASE_FREQ * t) +
                     2.0 * sin(2 * PI * HARMONIC_5 * t) +
                     1.5 * sin(2 * PI * HARMONIC_7 * t);
    }
}

int main() {
    printf("=== 整流器输入电流谐波抑制算法模拟 ===\n\n");
    
    // 1. 生成模拟信号
    double samples[SAMPLE_POINTS];
    generate_signal(samples, SAMPLE_POINTS);
    
    printf("1. 原始信号特征:\n");
    printf("   - 基波: 50Hz, 幅值10A\n");
    printf("   - 5次谐波: 250Hz, 幅值2A\n");
    printf("   - 7次谐波: 350Hz, 幅值1.5A\n");
    printf("   - 采样频率: %.0f Hz, 采样点数: %d\n\n", SAMPLE_RATE, SAMPLE_POINTS);
    
    // 2. 谐波分析
    HarmonicAnalysis analysis = analyze_harmonics(samples, SAMPLE_POINTS);
    
    printf("2. 分析得到的谐波幅值:\n");
    printf("   - 基波幅值: %.3f A\n", analysis.fundamental);
    printf("   - 5次谐波幅值: %.3f A\n", analysis.harmonic5);
    printf("   - 7次谐波幅值: %.3f A\n\n", analysis.harmonic7);
    
    // 3. 补偿计算
    CompensationResult compensation = calculate_compensation(analysis);
    
    printf("3. 计算出的补偿电流参考值:\n");
    printf("   - 5次谐波补偿电流: %.3f A\n", compensation.comp_current5);
    printf("   - 7次谐波补偿电流: %.3f A\n\n", compensation.comp_current7);
    
    // 4. 验证补偿效果
    printf("4. 补偿效果验证:\n");
    printf("   - 5次谐波抑制前: %.3f A, 抑制后: %.3f A\n", 
           analysis.harmonic5, analysis.harmonic5 - compensation.comp_current5);
    printf("   - 7次谐波抑制前: %.3f A, 抑制后: %.3f A\n", 
           analysis.harmonic7, analysis.harmonic7 - compensation.comp_current7);
    
    // 检查是否达到目标
    double after_comp5 = analysis.harmonic5 - compensation.comp_current5;
    double after_comp7 = analysis.harmonic7 - compensation.comp_current7;
    
    printf("\n5. 目标验证:\n");
    printf("   - 5次谐波目标<0.5A: %.3f A ", after_comp5);
    if (after_comp5 <= 0.5) {
        printf("[达标]\n");
    } else {
        printf("[未达标]\n");
    }
    
    printf("   - 7次谐波目标<0.3A: %.3f A ", after_comp7);
    if (after_comp7 <= 0.3) {
        printf("[达标]\n");
    } else {
        printf("[未达标]\n");
    }
    
    return 0;
}