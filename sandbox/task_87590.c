#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define OVER_CURRENT_THRESHOLD 5.0
#define SAMPLE_RATE 10000
#define PI 3.14159265358979323846

double calculate_rms(const double *samples, int n) {
    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        sum += samples[i] * samples[i];
    }
    return sqrt(sum / n);
}

void generate_sine_wave(double *samples, int n, double amplitude, double frequency) {
    for (int i = 0; i < n; i++) {
        double t = (double)i / SAMPLE_RATE;
        samples[i] = amplitude * sin(2 * PI * frequency * t);
    }
}

void generate_square_wave(double *samples, int n, double amplitude, double frequency) {
    int half_period_samples = (int)(SAMPLE_RATE / (2 * frequency));
    int samples_per_period = (int)(SAMPLE_RATE / frequency);
    
    for (int i = 0; i < n; i++) {
        int position_in_period = i % samples_per_period;
        if (position_in_period < half_period_samples) {
            samples[i] = amplitude;
        } else {
            samples[i] = -amplitude;
        }
    }
}

int main() {
    const int num_samples = 100;
    double samples[num_samples];
    
    printf("Test case 1: ");
    generate_sine_wave(samples, num_samples, 3.0, 50.0);
    double rms1 = calculate_rms(samples, num_samples);
    if (rms1 > OVER_CURRENT_THRESHOLD) {
        printf("ALARM: Overcurrent detected! RMS = %.2f A\n", rms1);
    } else {
        printf("Normal operation. RMS = %.2f A\n", rms1);
    }
    
    printf("Test case 2: ");
    generate_sine_wave(samples, num_samples, 7.0, 50.0);
    double rms2 = calculate_rms(samples, num_samples);
    if (rms2 > OVER_CURRENT_THRESHOLD) {
        printf("ALARM: Overcurrent detected! RMS = %.2f A\n", rms2);
    } else {
        printf("Normal operation. RMS = %.2f A\n", rms2);
    }
    
    printf("Test case 3: ");
    generate_square_wave(samples, num_samples, 6.0, 50.0);
    double rms3 = calculate_rms(samples, num_samples);
    if (rms3 > OVER_CURRENT_THRESHOLD) {
        printf("ALARM: Overcurrent detected! RMS = %.2f A\n", rms3);
    } else {
        printf("Normal operation. RMS = %.2f A\n", rms3);
    }
    
    return 0;
}