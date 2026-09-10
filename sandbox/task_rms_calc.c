#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define PI 3.14159265358979323846

double calculate_rms(const double samples[], int length) {
    if (length <= 0) {
        return 0.0;
    }
    
    double sum_squares = 0.0;
    for (int i = 0; i < length; i++) {
        sum_squares += samples[i] * samples[i];
    }
    
    return sqrt(sum_squares / length);
}

void generate_sine_wave(double samples[], int length, double amplitude, double frequency, double sampling_rate, double dc_offset) {
    double angular_freq = 2.0 * PI * frequency;
    double time_step = 1.0 / sampling_rate;
    
    for (int i = 0; i < length; i++) {
        double t = i * time_step;
        samples[i] = dc_offset + amplitude * sin(angular_freq * t);
    }
}

void generate_sine_with_harmonics(double samples[], int length, double amplitude, double fundamental_freq, 
                                  double sampling_rate, double dc_offset, double harmonic2_ratio, double harmonic3_ratio) {
    double angular_freq1 = 2.0 * PI * fundamental_freq;
    double angular_freq2 = 2.0 * PI * fundamental_freq * 2.0;
    double angular_freq3 = 2.0 * PI * fundamental_freq * 3.0;
    double time_step = 1.0 / sampling_rate;
    
    for (int i = 0; i < length; i++) {
        double t = i * time_step;
        samples[i] = dc_offset + 
                     amplitude * sin(angular_freq1 * t) +
                     amplitude * harmonic2_ratio * sin(angular_freq2 * t) +
                     amplitude * harmonic3_ratio * sin(angular_freq3 * t);
    }
}

int main() {
    const int SAMPLE_COUNT = 20;
    const double SAMPLING_RATE = 1000.0;
    const double FREQUENCY = 50.0;
    const double AMPLITUDE = 100.0;
    
    double samples[SAMPLE_COUNT];
    
    printf("继电保护有效值(RMS)计算算法测试\n");
    printf("================================\n");
    
    // 测试1: 纯正弦波
    generate_sine_wave(samples, SAMPLE_COUNT, AMPLITUDE, FREQUENCY, SAMPLING_RATE, 0.0);
    double rms1 = calculate_rms(samples, SAMPLE_COUNT);
    printf("测试1 - 纯正弦波(50Hz, 峰值100A): RMS = %.6f\n", rms1);
    printf("理论值: 100/√2 = %.6f\n", AMPLITUDE / sqrt(2.0));
    
    // 测试2: 正弦波带直流偏置
    generate_sine_wave(samples, SAMPLE_COUNT, AMPLITUDE, FREQUENCY, SAMPLING_RATE, 20.0);
    double rms2 = calculate_rms(samples, SAMPLE_COUNT);
    printf("\n测试2 - 正弦波带直流偏置20A: RMS = %.6f\n", rms2);
    printf("理论值: sqrt(20² + (100/√2)²) = %.6f\n", sqrt(20.0*20.0 + (AMPLITUDE/sqrt(2.0))*(AMPLITUDE/sqrt(2.0))));
    
    // 测试3: 含谐波的正弦波
    generate_sine_with_harmonics(samples, SAMPLE_COUNT, AMPLITUDE, FREQUENCY, SAMPLING_RATE, 
                                 0.0, 0.2, 0.1);
    double rms3 = calculate_rms(samples, SAMPLE_COUNT);
    printf("\n测试3 - 含谐波(20%%二次谐波, 10%%三次谐波): RMS = %.6f\n", rms3);
    
    // 测试4: 直流信号
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        samples[i] = 50.0;
    }
    double rms4 = calculate_rms(samples, SAMPLE_COUNT);
    printf("\n测试4 - 直流信号50A: RMS = %.6f\n", rms4);
    printf("理论值: 50.0\n");
    
    return 0;
}