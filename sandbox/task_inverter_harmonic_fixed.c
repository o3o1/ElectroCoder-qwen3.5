#include <stdio.h>
#include <math.h>
#include <string.h>

#define PI 3.14159265358979323846

void repetitive_control(double *error_buffer, int buffer_len, double *output, double k_r) {
    static int write_idx = 0;
    static double prev_output = 0.0;
    
    double current_error = error_buffer[write_idx];
    double new_output = k_r * (prev_output + current_error) + (1 - k_r) * prev_output;
    
    *output = new_output;
    prev_output = new_output;
    
    write_idx = (write_idx + 1) % buffer_len;
}

int main() {
    double fs = 10000.0;
    double f0 = 50.0;
    int buffer_len = (int)(fs / f0);
    
    double error_buffer[buffer_len];
    memset(error_buffer, 0, sizeof(error_buffer));
    
    int total_samples = 5 * buffer_len;
    double Ts = 1.0 / fs;
    
    int write_idx = 0;
    double output;
    
    for (int cycle = 0; cycle < 5; cycle++) {
        double max_output = -1e9;
        double min_output = 1e9;
        
        for (int i = 0; i < buffer_len; i++) {
            double t = (cycle * buffer_len + i) * Ts;
            double error = 0.1 * sin(2 * PI * 50 * t) + 0.05 * sin(2 * PI * 150 * t);
            
            error_buffer[write_idx] = error;
            
            repetitive_control(error_buffer, buffer_len, &output, 0.5);
            
            if (output > max_output) max_output = output;
            if (output < min_output) min_output = output;
            
            write_idx = (write_idx + 1) % buffer_len;
        }
        
        printf("Cycle %d: Max output = %.6f, Min output = %.6f\n", 
               cycle + 1, max_output, min_output);
    }
    
    return 0;
}