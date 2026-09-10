#include <stdio.h>
#include <math.h>

#define PI 3.14159265358979323846

float calculate_rms(float* samples, int length) {
    float sum_squares = 0.0f;
    for (int i = 0; i < length; i++) {
        sum_squares += samples[i] * samples[i];
    }
    return sqrt(sum_squares / length);
}

int main() {
    const int samples_per_cycle = 32;
    const int cycles = 2;
    const int total_samples = samples_per_cycle * cycles;
    const float peak_current = 10.0f;
    const float frequency = 50.0f;
    const float sampling_freq = 1600.0f;
    
    float current_samples[total_samples];
    
    for (int i = 0; i < total_samples; i++) {
        float time = i / sampling_freq;
        current_samples[i] = peak_current * sin(2 * PI * frequency * time);
    }
    
    float rms_value = calculate_rms(current_samples, total_samples);
    
    printf("AC Current RMS Calculation Result:\n");
    printf("Number of samples: %d\n", total_samples);
    printf("Calculated RMS value: %.6f A\n", rms_value);
    printf("Theoretical RMS value: %.6f A\n", peak_current / sqrt(2.0f));
    printf("Error: %.6f A\n", fabs(rms_value - peak_current / sqrt(2.0f)));
    
    return 0;
}