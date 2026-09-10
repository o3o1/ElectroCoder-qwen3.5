#include <stdio.h>
#include <math.h>

double calculate_true_rms(const double samples[], int n) {
    double sum_squares = 0.0;
    
    for (int i = 0; i < n; i++) {
        double rectified = fabs(samples[i]);
        sum_squares += rectified * rectified;
    }
    
    double mean_square = sum_squares / n;
    return sqrt(mean_square);
}

int main() {
    // sine wave: 10 samples (1 full cycle), amplitude 5V
    double sine_wave[10];
    for (int i = 0; i < 10; i++) {
        sine_wave[i] = 5.0 * sin(2.0 * M_PI * i / 10.0);
    }
    
    // square wave: 8 samples (2 cycles), high +3V, low -3V
    double square_wave[8] = {3.0, 3.0, -3.0, -3.0, 3.0, 3.0, -3.0, -3.0};
    
    // triangle wave: 12 samples (1.5 cycles), peak ±4V
    double triangle_wave[12];
    for (int i = 0; i < 12; i++) {
        int quarter = i % 4;
        if (quarter == 0) {
            triangle_wave[i] = 4.0 * (i % 8) / 4.0;
        } else if (quarter == 1) {
            triangle_wave[i] = 4.0 - 8.0 * ((i % 8) - 4) / 4.0;
        } else if (quarter == 2) {
            triangle_wave[i] = -4.0 * ((i % 8) - 4) / 4.0;
        } else {
            triangle_wave[i] = -4.0 + 8.0 * ((i % 8) - 8) / 4.0;
        }
    }
    
    double rms_sine = calculate_true_rms(sine_wave, 10);
    double rms_square = calculate_true_rms(square_wave, 8);
    double rms_triangle = calculate_true_rms(triangle_wave, 12);
    
    printf("sine wave: true rms = %.3f V\n", rms_sine);
    printf("square wave: true rms = %.3f V\n", rms_square);
    printf("triangle wave: true rms = %.3f V\n", rms_triangle);
    
    return 0;
}