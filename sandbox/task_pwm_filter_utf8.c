#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define PI 3.14159265358979323846

typedef struct {
    uint32_t switching_freq;
    uint32_t carrier_freq;
    uint8_t modulation_index;
    float* filter_coeffs;
    uint8_t filter_order;
} PWM_Config;

void apply_software_filter(PWM_Config* config, float* raw_samples, float* filtered_samples, uint32_t sample_count) {
    uint8_t order = config->filter_order;
    float* coeffs = config->filter_coeffs;
    
    for (uint32_t n = 0; n < sample_count; n++) {
        float sum = 0.0f;
        
        for (uint8_t k = 0; k < order; k++) {
            if (n >= k) {
                sum += coeffs[k] * raw_samples[n - k];
            }
        }
        
        filtered_samples[n] = sum;
    }
}

int main() {
    float filter_coeffs[] = {0.25f, 0.5f, 0.25f};
    uint8_t filter_order = 3;
    
    PWM_Config config = {
        .switching_freq = 20000,
        .carrier_freq = 1000000,
        .modulation_index = 80,
        .filter_coeffs = filter_coeffs,
        .filter_order = filter_order
    };
    
    uint32_t sample_count = 100;
    float raw_samples[100];
    float filtered_samples[100];
    
    for (uint32_t i = 0; i < sample_count; i++) {
        float t = (float)i / 1000.0f;
        float signal = sin(2.0f * PI * 1000.0f * t);
        float noise = 0.1f * sin(2.0f * PI * 50000.0f * t);
        raw_samples[i] = signal + noise;
    }
    
    apply_software_filter(&config, raw_samples, filtered_samples, sample_count);
    
    for (uint32_t i = 0; i < 10; i++) {
        printf("原始采样[%u]=%.6f, 滤波后[%u]=%.6f\n", i, raw_samples[i], i, filtered_samples[i]);
    }
    
    return 0;
}
