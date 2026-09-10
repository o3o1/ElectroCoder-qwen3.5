#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define PI 3.14159265358979323846

void design_fir_highpass(float *coeffs, int N, float fc, float fs) {
    int M = (N - 1) / 2;
    
    for (int n = 0; n < N; n++) {
        float h_d;
        int m = n - M;
        
        if (m == 0) {
            h_d = 1.0 - (2.0 * fc / fs);
        } else {
            float theta = 2.0 * PI * fc * m / fs;
            h_d = -sin(theta) / (PI * m);
        }
        
        float w;
        if (N == 1) {
            w = 1.0;
        } else {
            float n_norm = (float)n / (N - 1);
            w = 0.42 - 0.5 * cos(2.0 * PI * n_norm) + 0.08 * cos(4.0 * PI * n_norm);
        }
        
        coeffs[n] = h_d * w;
    }
}

float calculate_stopband_attenuation(float *coeffs, int N, float fc, float fs, float stopband_end) {
    int num_points = 100;
    float max_response = 0.0;
    
    for (int i = 0; i <= num_points; i++) {
        float f = stopband_end * i / num_points;
        float real_sum = 0.0;
        float imag_sum = 0.0;
        
        for (int k = 0; k < N; k++) {
            float angle = -2.0 * PI * f * k / fs;
            real_sum += coeffs[k] * cos(angle);
            imag_sum += coeffs[k] * sin(angle);
        }
        
        float magnitude = sqrt(real_sum * real_sum + imag_sum * imag_sum);
        if (magnitude > max_response) {
            max_response = magnitude;
        }
    }
    
    return 20.0 * log10(max_response);
}

void test_case(float *coeffs, int N, float fc, float fs, const char *name) {
    printf("\n=== Test Case: %s ===\n", name);
    printf("Parameters: N=%d, fc=%.1f Hz, fs=%.1f Hz\n", N, fc, fs);
    
    design_fir_highpass(coeffs, N, fc, fs);
    
    printf("First 5 coefficients:\n");
    for (int i = 0; i < 5 && i < N; i++) {
        printf("coeff[%2d] = %+.6f\n", i, coeffs[i]);
    }
    
    printf("Last 5 coefficients:\n");
    for (int i = N - 5; i < N; i++) {
        if (i >= 0) {
            printf("coeff[%2d] = %+.6f\n", i, coeffs[i]);
        }
    }
    
    float stopband_end = fc - 200.0;
    if (stopband_end < 0) stopband_end = 0;
    float attenuation = calculate_stopband_attenuation(coeffs, N, fc, fs, stopband_end);
    printf("Stopband attenuation (0-%.1f Hz): %.2f dB\n", stopband_end, attenuation);
}

int main() {
    int N1 = 31;
    float fc1 = 1000.0;
    float fs1 = 8000.0;
    
    float *coeffs1 = (float*)malloc(N1 * sizeof(float));
    
    printf("=== Main Test Case ===\n");
    printf("Parameters: N=%d, fc=%.1f Hz, fs=%.1f Hz\n", N1, fc1, fs1);
    
    design_fir_highpass(coeffs1, N1, fc1, fs1);
    
    printf("First 5 coefficients:\n");
    for (int i = 0; i < 5; i++) {
        printf("coeff[%2d] = %+.6f\n", i, coeffs1[i]);
    }
    
    printf("Last 5 coefficients:\n");
    for (int i = N1 - 5; i < N1; i++) {
        printf("coeff[%2d] = %+.6f\n", i, coeffs1[i]);
    }
    
    float stopband_end = fc1 - 200.0;
    float attenuation = calculate_stopband_attenuation(coeffs1, N1, fc1, fs1, stopband_end);
    printf("Stopband attenuation (0-%.1f Hz): %.2f dB\n", stopband_end, attenuation);
    
    test_case(coeffs1, 15, 1500.0, 10000.0, "Lower order, higher cutoff");
    
    float *coeffs2 = (float*)malloc(51 * sizeof(float));
    test_case(coeffs2, 51, 500.0, 16000.0, "Higher order, lower cutoff");
    
    free(coeffs1);
    free(coeffs2);
    
    return 0;
}