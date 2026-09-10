#include <stdio.h>
#include <math.h>

typedef struct {
    float target_freq;
    float estimated_freq;
    int mode;
    float stable_k;
    float fast_k;
    float threshold;
} PLL_Controller;

void pll_init(PLL_Controller *pll, float target, float stable_k, float fast_k, float th) {
    pll->target_freq = target;
    pll->estimated_freq = target;
    pll->mode = 0;
    pll->stable_k = stable_k;
    pll->fast_k = fast_k;
    pll->threshold = th;
}

void pll_update(PLL_Controller *pll, float measured_freq) {
    float error = measured_freq - pll->target_freq;
    float abs_error = fabsf(error);
    
    if (abs_error > pll->threshold) {
        pll->mode = 1;
    } else {
        pll->mode = 0;
    }
    
    float k = (pll->mode == 1) ? pll->fast_k : pll->stable_k;
    pll->estimated_freq += k * (measured_freq - pll->estimated_freq);
}

int main() {
    PLL_Controller pll;
    pll_init(&pll, 50.0, 0.1, 0.5, 2.0);
    
    float measurements[] = {48.0, 49.0, 50.5, 51.0, 50.0};
    int steps = sizeof(measurements) / sizeof(measurements[0]);
    
    for (int i = 0; i < steps; i++) {
        pll_update(&pll, measurements[i]);
        printf("Step %d: meas=%.2f, est=%.2f, mode=%d\n", 
               i+1, measurements[i], pll.estimated_freq, pll.mode);
    }
    
    return 0;
}