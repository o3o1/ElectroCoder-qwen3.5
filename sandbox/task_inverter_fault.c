#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define ADC_SAMPLES 64
#define ADC_MAX_VOLT 311.0f
#define PI 3.14159265358979323846f

int detect_distortion(float* voltage_samples, int n, float threshold) {
    if (n <= 0 || threshold <= 0) return 0;
    
    float max_amplitude = 0.0f;
    for (int i = 0; i < n; i++) {
        float abs_val = fabsf(voltage_samples[i]);
        if (abs_val > max_amplitude) {
            max_amplitude = abs_val;
        }
    }
    
    if (max_amplitude == 0.0f) return 0;
    
    float positive_error_sum = 0.0f;
    float negative_error_sum = 0.0f;
    int positive_count = 0;
    int negative_count = 0;
    
    for (int i = 0; i < n; i++) {
        float ideal = max_amplitude * sinf(2 * PI * i / n);
        float actual = voltage_samples[i];
        
        if (actual >= 0) {
            float error = fabsf(actual - ideal) / max_amplitude;
            positive_error_sum += error;
            positive_count++;
        } else {
            float error = fabsf(actual - ideal) / max_amplitude;
            negative_error_sum += error;
            negative_count++;
        }
    }
    
    float positive_avg_error = positive_count > 0 ? positive_error_sum / positive_count : 0.0f;
    float negative_avg_error = negative_count > 0 ? negative_error_sum / negative_count : 0.0f;
    
    int positive_distorted = positive_avg_error > threshold ? 1 : 0;
    int negative_distorted = negative_avg_error > threshold ? 1 : 0;
    
    if (positive_distorted && negative_distorted) return 2;
    if (positive_distorted) return 1;
    if (negative_distorted) return -1;
    return 0;
}

int main() {
    float test1[ADC_SAMPLES];
    float test2[ADC_SAMPLES];
    float test3[ADC_SAMPLES];
    float test4[ADC_SAMPLES];
    
    float amplitude = ADC_MAX_VOLT;
    
    for (int i = 0; i < ADC_SAMPLES; i++) {
        float angle = 2 * PI * i / ADC_SAMPLES;
        test1[i] = amplitude * sinf(angle);
        
        test2[i] = amplitude * sinf(angle);
        if (test2[i] > 0.7f * amplitude) {
            test2[i] = 0.7f * amplitude;
        }
        
        test3[i] = amplitude * sinf(angle);
        if (test3[i] < -0.6f * amplitude) {
            test3[i] = -0.6f * amplitude;
        }
        
        test4[i] = amplitude * sinf(angle);
        if (test4[i] > 0) {
            test4[i] *= 0.8f;
        } else {
            test4[i] *= 1.2f;
        }
    }
    
    float threshold = 0.1f;
    
    printf("Test 1 - Ideal sine wave: %d\n", detect_distortion(test1, ADC_SAMPLES, threshold));
    printf("Test 2 - Positive half-wave clipping: %d\n", detect_distortion(test2, ADC_SAMPLES, threshold));
    printf("Test 3 - Negative half-wave limiting: %d\n", detect_distortion(test3, ADC_SAMPLES, threshold));
    printf("Test 4 - Asymmetric distortion: %d\n", detect_distortion(test4, ADC_SAMPLES, threshold));
    
    return 0;
}