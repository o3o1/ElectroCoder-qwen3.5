#include <stdio.h>
#include <math.h>

float calculate_rms(const float* samples, int num_samples) {
    if (samples == NULL || num_samples <= 0) {
        return 0.0f;
    }
    
    float sum_squares = 0.0f;
    for (int i = 0; i < num_samples; i++) {
        sum_squares += samples[i] * samples[i];
    }
    
    return sqrt(sum_squares / num_samples);
}

int main() {
    const int SAMPLES_PER_CYCLE = 32;
    const float FREQUENCY = 50.0f;
    const float SAMPLE_RATE = 1600.0f;
    const float PI = 3.14159265358979323846f;
    
    float samples1[SAMPLES_PER_CYCLE];
    float samples2[SAMPLES_PER_CYCLE];
    
    for (int i = 0; i < SAMPLES_PER_CYCLE; i++) {
        float t = i / SAMPLE_RATE;
        samples1[i] = 100.0f * sin(2.0f * PI * FREQUENCY * t);
        samples2[i] = 50.0f + 50.0f * sin(2.0f * PI * FREQUENCY * t);
    }
    
    float rms1 = calculate_rms(samples1, SAMPLES_PER_CYCLE);
    float rms2 = calculate_rms(samples2, SAMPLES_PER_CYCLE);
    
    printf("Pure sine wave RMS: %.4f A (Theoretical: 70.7107 A)\n", rms1);
    printf("DC offset wave RMS: %.4f A (Theoretical: 61.2372 A)\n", rms2);
    
    return 0;
}