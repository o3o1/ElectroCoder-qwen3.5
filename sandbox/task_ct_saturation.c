#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define PI 3.14159265358979323846
#define SAMPLE_RATE 1600  // 32点 * 50Hz = 1600Hz

int detect_ct_saturation(float* samples, int n, float threshold) {
    if (n <= 0 || samples == NULL) {
        return 0;
    }
    
    float max_val = samples[0];
    float min_val = samples[0];
    
    for (int i = 0; i < n; i++) {
        if (samples[i] > max_val) {
            max_val = samples[i];
        }
        if (samples[i] < min_val) {
            min_val = samples[i];
        }
    }
    
    if (max_val > threshold && min_val < -threshold) {
        return 1;
    }
    else if (max_val > threshold) {
        return 1;
    }
    else if (min_val < -threshold) {
        return -1;
    }
    else {
        return 0;
    }
}

void generate_sine_wave(float* buffer, int n, float amplitude, float frequency) {
    for (int i = 0; i < n; i++) {
        float t = (float)i / SAMPLE_RATE;
        buffer[i] = amplitude * sin(2 * PI * frequency * t);
    }
}

void generate_positive_saturation(float* buffer, int n, float amplitude, float frequency) {
    float threshold = 1.0f;
    for (int i = 0; i < n; i++) {
        float t = (float)i / SAMPLE_RATE;
        float value = amplitude * sin(2 * PI * frequency * t);
        if (value > threshold) {
            value = threshold;
        }
        buffer[i] = value;
    }
}

void generate_negative_saturation(float* buffer, int n, float amplitude, float frequency) {
    float threshold = -1.0f;
    for (int i = 0; i < n; i++) {
        float t = (float)i / SAMPLE_RATE;
        float value = amplitude * sin(2 * PI * frequency * t);
        if (value < threshold) {
            value = threshold;
        }
        buffer[i] = value;
    }
}

int main() {
    const int n = 32;
    float samples_normal[n];
    float samples_pos_sat[n];
    float samples_neg_sat[n];
    
    float threshold = 1.2f;
    
    generate_sine_wave(samples_normal, n, 0.8f, 50.0f);
    generate_positive_saturation(samples_pos_sat, n, 1.8f, 50.0f);
    generate_negative_saturation(samples_neg_sat, n, 1.6f, 50.0f);
    
    int result1 = detect_ct_saturation(samples_normal, n, threshold);
    int result2 = detect_ct_saturation(samples_pos_sat, n, threshold);
    int result3 = detect_ct_saturation(samples_neg_sat, n, threshold);
    
    printf("Test 1: Normal waveform -> Saturation status: %d\n", result1);
    printf("Test 2: Positive saturation -> Saturation status: %d\n", result2);
    printf("Test 3: Negative saturation -> Saturation status: %d\n", result3);
    
    return 0;
}