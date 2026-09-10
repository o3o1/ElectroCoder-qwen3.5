#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#define PI 3.14159265358979323846

void fft(double complex x[], int n) {
    if (n <= 1) return;
    
    double complex even[n/2];
    double complex odd[n/2];
    
    for (int i = 0; i < n/2; i++) {
        even[i] = x[2*i];
        odd[i] = x[2*i + 1];
    }
    
    fft(even, n/2);
    fft(odd, n/2);
    
    for (int k = 0; k < n/2; k++) {
        double complex t = cexp(-I * 2.0 * PI * k / n) * odd[k];
        x[k] = even[k] + t;
        x[k + n/2] = even[k] - t;
    }
}

void harmonic_analysis(double complex fft_result[], int n, double fundamental_freq) {
    double fs = fundamental_freq * n;
    double df = fs / n;
    
    printf("谐波分析结果（幅值，单位：V）：\n");
    
    for (int h = 1; h <= 15; h++) {
        double freq = h * fundamental_freq;
        int index = (int)round(freq / df);
        
        if (index >= 0 && index < n/2) {
            double amplitude = 2.0 * cabs(fft_result[index]) / n;
            printf("第%d次谐波（%.0f Hz）：%.3f V\n", h, freq, amplitude);
        }
    }
}

int main() {
    int N = 256;
    double fs = 12800.0;
    double T = 1.0 / fs;
    
    double complex signal[N];
    
    for (int i = 0; i < N; i++) {
        double t = i * T;
        signal[i] = 1.0 * sin(2 * PI * 50 * t) + 
                    0.3 * sin(2 * PI * 150 * t) + 
                    0.2 * sin(2 * PI * 250 * t) + 
                    0.1 * sin(2 * PI * 350 * t);
    }
    
    fft(signal, N);
    
    printf("前8个频率分量的幅值（单位：V）：\n");
    for (int i = 0; i < 8; i++) {
        double amplitude = 2.0 * cabs(signal[i]) / N;
        double freq = i * fs / N;
        printf("频率 %.1f Hz：%.3f V\n", freq, amplitude);
    }
    
    printf("\n");
    harmonic_analysis(signal, N, 50.0);
    
    return 0;
}