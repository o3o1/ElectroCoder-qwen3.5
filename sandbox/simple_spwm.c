#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void generate_spwm(float amplitude, float frequency, float carrier_freq, int samples, float duty_cycle[]) {
    if (amplitude < 0.0f) amplitude = 0.0f;
    if (amplitude > 1.0f) amplitude = 1.0f;
    
    for (int n = 0; n < samples; n++) {
        float t = (float)n / carrier_freq;
        float sine_value = amplitude * sinf(2.0f * M_PI * frequency * t);
        float duty = 0.5f + 0.5f * sine_value;
        
        if (duty < 0.0f) duty = 0.0f;
        if (duty > 1.0f) duty = 1.0f;
        
        duty_cycle[n] = duty;
    }
}

int main() {
    float amplitude = 0.8f;
    float frequency = 50.0f;
    float carrier_freq = 10000.0f;
    int samples = 100;
    
    float duty_cycle[100];
    
    generate_spwm(amplitude, frequency, carrier_freq, samples, duty_cycle);
    
    printf("First 10 duty cycle values:\n");
    for (int i = 0; i < 10; i++) {
        printf("duty_cycle[%d] = %.3f\n", i, duty_cycle[i]);
    }
    
    float min_val = 1.0f;
    float max_val = 0.0f;
    float sum = 0.0f;
    
    for (int i = 0; i < samples; i++) {
        if (duty_cycle[i] < min_val) min_val = duty_cycle[i];
        if (duty_cycle[i] > max_val) max_val = duty_cycle[i];
        sum += duty_cycle[i];
    }
    
    float avg = sum / samples;
    
    printf("\nStatistics:\n");
    printf("Max value: %.3f\n", max_val);
    printf("Min value: %.3f\n", min_val);
    printf("Average: %.3f\n", avg);
    
    return 0;
}