#include <stdio.h>
#include <math.h>

#define PI 3.141592653589793

double pll_wave_shaper(double phase, int wave_mode) {
    if (wave_mode == 0) {
        if (phase >= 0 && phase < PI) {
            return 1.0;
        } else {
            return -1.0;
        }
    } else {
        return sin(phase);
    }
}

int main() {
    double test_phases[] = {0.0, PI/4, PI/2, 3*PI/4, PI, 5*PI/4, 3*PI/2, 7*PI/4};
    int num_points = 8;
    
    printf("=== Square Wave Test ===\n");
    for (int i = 0; i < num_points; i++) {
        double phase = test_phases[i];
        double output = pll_wave_shaper(phase, 0);
        printf("Phase: %.2f rad -> Output: %.2f\n", phase, output);
    }
    
    printf("\n=== Sine Wave Test ===\n");
    for (int i = 0; i < num_points; i++) {
        double phase = test_phases[i];
        double output = pll_wave_shaper(phase, 1);
        printf("Phase: %.2f rad -> Output: %.2f\n", phase, output);
    }
    
    return 0;
}