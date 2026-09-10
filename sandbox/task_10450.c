#include <stdio.h>
#include <math.h>
#include <string.h>

#define PI 3.14159265358979323846
#define WINDOW_SIZE 32
#define SAMPLE_RATE 1600.0
#define FREQUENCY 50.0

double calculate_rms(double* buffer, int window_size) {
    double sum_squares = 0.0;
    for (int i = 0; i < window_size; i++) {
        sum_squares += buffer[i] * buffer[i];
    }
    return sqrt(sum_squares / window_size);
}

void update_buffer(double* buffer, int window_size, double new_sample) {
    static int write_index = 0;
    
    buffer[write_index] = new_sample;
    write_index = (write_index + 1) % window_size;
}

void simulate_sine_wave(double* buffer, double amplitude, double dc_offset, int num_samples) {
    for (int i = 0; i < num_samples; i++) {
        double t = i / SAMPLE_RATE;
        double sample = amplitude * sin(2 * PI * FREQUENCY * t) + dc_offset;
        update_buffer(buffer, WINDOW_SIZE, sample);
        
        if (i >= WINDOW_SIZE - 1) {
            double rms = calculate_rms(buffer, WINDOW_SIZE);
            if (i == num_samples - 1) {
                printf("RMS: %.6f A\n", rms);
            }
        }
    }
}

int main() {
    double buffer[WINDOW_SIZE];
    memset(buffer, 0, sizeof(buffer));
    
    printf("Test 1: Pure sine wave with amplitude 10A\n");
    simulate_sine_wave(buffer, 10.0, 0.0, 100);
    
    printf("\nTest 2: Pure sine wave with amplitude 5A\n");
    memset(buffer, 0, sizeof(buffer));
    simulate_sine_wave(buffer, 5.0, 0.0, 100);
    
    printf("\nTest 3: Sine wave with amplitude 10A + 3A DC offset\n");
    memset(buffer, 0, sizeof(buffer));
    simulate_sine_wave(buffer, 10.0, 3.0, 100);
    
    return 0;
}