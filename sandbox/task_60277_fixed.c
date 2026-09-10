#include <stdio.h>
#include <math.h>

#define PI 3.14159265358979323846
#define SAMPLE_RATE 1600.0
#define FREQUENCY 50.0
#define SAMPLES_PER_CYCLE 32

double calculate_rms(float samples[], int length) {
    double sum_squares = 0.0;
    
    for (int i = 0; i < length; i++) {
        sum_squares += (double)samples[i] * (double)samples[i];
    }
    
    return sqrt(sum_squares / length);
}

int main() {
    float sine_wave[SAMPLES_PER_CYCLE];
    float harmonic_wave[SAMPLES_PER_CYCLE];
    
    for (int i = 0; i < SAMPLES_PER_CYCLE; i++) {
        double t = (double)i / SAMPLE_RATE;
        double angle = 2.0 * PI * FREQUENCY * t;
        
        sine_wave[i] = 10.0 * sin(angle);
        
        harmonic_wave[i] = 10.0 * sin(angle) + 2.0 * sin(3.0 * angle);
    }
    
    double sine_rms = calculate_rms(sine_wave, SAMPLES_PER_CYCLE);
    double harmonic_rms = calculate_rms(harmonic_wave, SAMPLES_PER_CYCLE);
    
    printf("标准正弦波有效值: %.3f A\n", sine_rms);
    printf("含谐波波形有效值: %.3f A\n", harmonic_rms);
    
    return 0;
}