#include <stdio.h>

double ema_filter(double new_sample, double prev_ema, double alpha) {
    return alpha * new_sample + (1.0 - alpha) * prev_ema;
}

int main() {
    double samples[] = {10.2, 10.5, 10.3, 10.8, 10.6};
    int num_samples = sizeof(samples) / sizeof(samples[0]);
    double alpha = 0.3;
    
    double ema = samples[0];
    double ema_sum = 0.0;
    
    for (int i = 0; i < num_samples; i++) {
        if (i == 0) {
            ema = samples[0];
        } else {
            ema = ema_filter(samples[i], ema, alpha);
        }
        
        printf("Sample[%d]=%.2f, EMA=%.2f\n", i, samples[i], ema);
        ema_sum += ema;
    }
    
    double average_ema = ema_sum / num_samples;
    printf("Average EMA = %.2f\n", average_ema);
    
    return 0;
}