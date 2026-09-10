#include <stdio.h>
#include <math.h>

#define PI 3.14159265358979323846

float iir_lowpass_filter(float input, float cutoff_freq, float sample_rate, float *prev_output) {
    float alpha = 2 * PI * cutoff_freq / (2 * PI * cutoff_freq + sample_rate);
    float output = alpha * input + (1 - alpha) * (*prev_output);
    *prev_output = output;
    return output;
}

int main() {
    float sample_rate = 1000.0f;
    int num_samples = 10;
    
    float fc100_prev = 0.0f;
    float fc150_prev = 0.0f;
    
    printf("Sample 0: raw=0.000, fc100=0.000, fc150=0.000\n");
    
    for (int i = 1; i < num_samples; i++) {
        float t = i / sample_rate;
        float raw_signal = sin(2 * PI * 50 * t) + sin(2 * PI * 200 * t);
        
        float fc100_out = iir_lowpass_filter(raw_signal, 100.0f, sample_rate, &fc100_prev);
        float fc150_out = iir_lowpass_filter(raw_signal, 150.0f, sample_rate, &fc150_prev);
        
        printf("Sample %d: raw=%.3f, fc100=%.3f, fc150=%.3f\n", 
               i, raw_signal, fc100_out, fc150_out);
    }
    
    return 0;
}