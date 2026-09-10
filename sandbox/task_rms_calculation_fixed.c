#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define ADC_CHANNEL_1 0
#define ADC_READ(ch) (mock_adc_read())

#define SAMPLE_FREQ 1600
#define POWER_FREQ 50
#define SAMPLES_PER_CYCLE (SAMPLE_FREQ / POWER_FREQ)
#define PI 3.14159265358979323846

static int mock_counter = 0;
static float* mock_samples = NULL;
static int mock_samples_count = 0;

float mock_adc_read(void) {
    if (mock_samples == NULL || mock_counter >= mock_samples_count) {
        mock_counter = 0;
        return 0.0f;
    }
    return mock_samples[mock_counter++];
}

void set_mock_samples(float* samples, int count) {
    mock_samples = samples;
    mock_samples_count = count;
    mock_counter = 0;
}

float calculate_rms(const float* samples, int num_samples) {
    if (num_samples <= 0 || samples == NULL) {
        return 0.0f;
    }
    
    float sum_squares = 0.0f;
    for (int i = 0; i < num_samples; i++) {
        sum_squares += samples[i] * samples[i];
    }
    
    return sqrtf(sum_squares / num_samples);
}

void generate_sine_wave(float* buffer, int num_samples, float amplitude) {
    for (int i = 0; i < num_samples; i++) {
        buffer[i] = amplitude * sin(2 * PI * i / num_samples);
    }
}

void generate_distorted_wave(float* buffer, int num_samples, float fundamental_amp, float harmonic3_amp) {
    for (int i = 0; i < num_samples; i++) {
        float angle = 2 * PI * i / num_samples;
        buffer[i] = fundamental_amp * sin(angle) + 
                   harmonic3_amp * sin(3 * angle);
    }
}

int main() {
    printf("=== Relay Protection RMS Calculation Simulation ===\n\n");
    
    float samples[SAMPLES_PER_CYCLE];
    
    printf("Test Case 1: Pure Sine Wave (Amplitude 100A)\n");
    generate_sine_wave(samples, SAMPLES_PER_CYCLE, 100.0f);
    
    float rms1 = calculate_rms(samples, SAMPLES_PER_CYCLE);
    printf("Calculated RMS: %.4f A\n", rms1);
    printf("Theoretical RMS: %.4f A\n", 100.0f / sqrtf(2.0f));
    printf("Error: %.4f A\n\n", fabs(rms1 - 100.0f / sqrtf(2.0f)));
    
    printf("Test Case 2: Distorted Wave (Fundamental 100A + 3rd Harmonic 20A)\n");
    generate_distorted_wave(samples, SAMPLES_PER_CYCLE, 100.0f, 20.0f);
    
    float rms2 = calculate_rms(samples, SAMPLES_PER_CYCLE);
    printf("Calculated RMS: %.4f A\n", rms2);
    
    float theoretical_rms2 = sqrtf((100.0f * 100.0f + 20.0f * 20.0f) / 2.0f);
    printf("Theoretical RMS: %.4f A\n", theoretical_rms2);
    printf("Error: %.4f A\n\n", fabs(rms2 - theoretical_rms2));
    
    printf("Simulated ADC Reading Test:\n");
    set_mock_samples(samples, SAMPLES_PER_CYCLE);
    
    float adc_samples[SAMPLES_PER_CYCLE];
    for (int i = 0; i < SAMPLES_PER_CYCLE; i++) {
        adc_samples[i] = ADC_READ(ADC_CHANNEL_1);
    }
    
    float rms_adc = calculate_rms(adc_samples, SAMPLES_PER_CYCLE);
    printf("RMS calculated via simulated ADC reading: %.4f A\n", rms_adc);
    
    return 0;
}