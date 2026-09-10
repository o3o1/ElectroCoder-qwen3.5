#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

float iir_lowpass_filter(float input, float alpha, float *prev_output) {
    float output = alpha * input + (1.0f - alpha) * (*prev_output);
    *prev_output = output;
    return output;
}

int main() {
    float fs = 1000.0f;
    float T = 1.0f / fs;
    float fc = 50.0f;
    
    float alpha = (2.0f * M_PI * fc * T) / (2.0f * M_PI * fc * T + 1.0f);
    
    float prev_output = 0.0f;
    
    printf("Sampling frequency: %.1f Hz\n", fs);
    printf("Cutoff frequency: %.1f Hz\n", fc);
    printf("Alpha coefficient: %.6f\n\n", alpha);
    
    printf("Index\tOriginal\tFiltered\n");
    printf("--------------------------------\n");
    
    for (int n = 0; n < 100; n++) {
        float x = sin(2.0f * M_PI * 10.0f * n * T) + 0.5f * sin(2.0f * M_PI * 100.0f * n * T);
        float y = iir_lowpass_filter(x, alpha, &prev_output);
        
        if (n < 10) {
            printf("%d\t%.6f\t%.6f\n", n, x, y);
        }
    }
    
    return 0;
}