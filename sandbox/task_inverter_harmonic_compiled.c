#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define PI 3.14159265358979323846
#define BASE_FREQ 50.0f

typedef struct {
    int order;
    float amplitude;
    float phase;
} HarmonicComponent;

void harmonic_analysis(float voltage[], int N, float fs, HarmonicComponent harmonics[]) {
    float f0 = BASE_FREQ;
    int orders[] = {1, 3, 5};
    float target_freqs[] = {f0, 3*f0, 5*f0};
    
    for (int h = 0; h < 3; h++) {
        int k = (int)(target_freqs[h] * N / fs + 0.5f);
        if (k < 0) k = 0;
        if (k >= N/2) k = N/2 - 1;
        
        float real = 0.0f;
        float imag = 0.0f;
        
        for (int n = 0; n < N; n++) {
            float angle = -2.0f * PI * k * n / N;
            real += voltage[n] * cos(angle);
            imag += voltage[n] * sin(angle);
        }
        
        float amplitude = sqrt(real*real + imag*imag) * 2.0f / N;
        float phase = atan2(imag, real);
        
        harmonics[h].order = orders[h];
        harmonics[h].amplitude = amplitude;
        harmonics[h].phase = phase;
    }
}

void generate_compensation(HarmonicComponent harmonics[], int N, float fs, float compensation[]) {
    float f0 = BASE_FREQ;
    
    for (int n = 0; n < N; n++) {
        compensation[n] = 0.0f;
        
        for (int h = 0; h < 3; h++) {
            float angle = 2.0f * PI * harmonics[h].order * f0 * n / fs + harmonics[h].phase;
            compensation[n] -= harmonics[h].amplitude * sin(angle);
        }
    }
}

int main() {
    int N = 200;
    float fs = 1000.0f;
    float f0 = BASE_FREQ;
    
    float voltage[N];
    
    for (int n = 0; n < N; n++) {
        float t = n / fs;
        voltage[n] = 311.0f * sin(2.0f * PI * f0 * t) +
                     15.0f * sin(2.0f * PI * 3 * f0 * t) +
                     8.0f * sin(2.0f * PI * 5 * f0 * t);
    }
    
    HarmonicComponent harmonics[3];
    
    harmonic_analysis(voltage, N, fs, harmonics);
    
    printf("谐波分析结果：\n");
    printf("次数\t幅值(V)\t相位(rad)\n");
    for (int i = 0; i < 3; i++) {
        printf("%d\t%.3f\t%.3f\n", 
               harmonics[i].order, 
               harmonics[i].amplitude,
               harmonics[i].phase);
    }
    
    float compensation[N];
    generate_compensation(harmonics, N, fs, compensation);
    
    printf("\n前10个补偿信号值：\n");
    for (int i = 0; i < 10; i++) {
        printf("compensation[%d] = %.3f\n", i, compensation[i]);
    }
    
    return 0;
}