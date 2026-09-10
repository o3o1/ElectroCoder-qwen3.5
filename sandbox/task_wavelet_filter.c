#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define PI 3.14159265358979323846
#define SAMPLE_POINTS 64
#define SAMPLE_FREQ 1280.0
#define FUND_FREQ 50.0
#define HARMONIC_FREQ 150.0
#define FUND_AMP 1.0
#define HARMONIC_AMP 0.3

void haar_wavelet_transform(float *signal, float *approx, float *detail, int n) {
    int m = n / 2;
    float sqrt2 = sqrtf(2.0f);
    
    for (int i = 0; i < m; i++) {
        approx[i] = (signal[2*i] + signal[2*i+1]) / sqrt2;
        detail[i] = (signal[2*i] - signal[2*i+1]) / sqrt2;
    }
}

void haar_wavelet_reconstruct(float *approx, float *detail, float *reconstructed, int n) {
    int m = n / 2;
    float sqrt2 = sqrtf(2.0f);
    
    for (int i = 0; i < m; i++) {
        reconstructed[2*i] = (approx[i] + detail[i]) / sqrt2;
        reconstructed[2*i+1] = (approx[i] - detail[i]) / sqrt2;
    }
}

void extract_harmonic(float *signal, float *harmonic, int n) {
    float approx[32];
    float detail[32];
    
    haar_wavelet_transform(signal, approx, detail, n);
    
    for (int i = 0; i < 32; i++) {
        approx[i] = 0.0f;
    }
    
    haar_wavelet_reconstruct(approx, detail, harmonic, n);
}

float calculate_rms(float *signal, int n) {
    float sum = 0.0f;
    for (int i = 0; i < n; i++) {
        sum += signal[i] * signal[i];
    }
    return sqrtf(sum / n);
}

int main() {
    float signal[SAMPLE_POINTS];
    float harmonic[SAMPLE_POINTS];
    float time;
    
    for (int i = 0; i < SAMPLE_POINTS; i++) {
        time = i / SAMPLE_FREQ;
        signal[i] = FUND_AMP * sinf(2.0f * PI * FUND_FREQ * time) +
                    HARMONIC_AMP * sinf(2.0f * PI * HARMONIC_FREQ * time);
    }
    
    extract_harmonic(signal, harmonic, SAMPLE_POINTS);
    
    printf("Original signal first 10 points:\n");
    for (int i = 0; i < 10; i++) {
        printf("signal[%d] = %.6f\n", i, signal[i]);
    }
    
    printf("\nExtracted harmonic signal first 10 points:\n");
    for (int i = 0; i < 10; i++) {
        printf("harmonic[%d] = %.6f\n", i, harmonic[i]);
    }
    
    float fundamental_rms = calculate_rms(signal, SAMPLE_POINTS);
    float harmonic_rms = calculate_rms(harmonic, SAMPLE_POINTS);
    float thd = (harmonic_rms / fundamental_rms) * 100.0f;
    
    printf("\nHarmonic Distortion Rate (THD): %.2f%%\n", thd);
    
    return 0;
}