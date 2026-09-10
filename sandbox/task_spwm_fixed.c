#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#define PI 3.14159265358979323846

typedef struct {
    float frequency;
    float carrier_freq;
    float amplitude;
    int samples_per_cycle;
} SPWM_Config;

void calculate_spwm_duty_cycles(SPWM_Config *config, float *duty_cycles) {
    float T = 1.0f / config->carrier_freq;
    
    for (int n = 0; n < config->samples_per_cycle; n++) {
        float t = n * T;
        float angle = 2.0f * PI * config->frequency * t;
        float duty = 0.5f + 0.5f * config->amplitude * sinf(angle);
        duty_cycles[n] = duty;
    }
}

bool validate_duty_cycles(float *duty_cycles, int count) {
    for (int i = 0; i < count; i++) {
        if (duty_cycles[i] < 0.0f || duty_cycles[i] > 1.0f) {
            return false;
        }
    }
    return true;
}

int main() {
    SPWM_Config config = {
        .frequency = 50.0f,
        .carrier_freq = 10000.0f,
        .amplitude = 0.8f,
        .samples_per_cycle = 200
    };
    
    float duty_cycles[200];
    
    calculate_spwm_duty_cycles(&config, duty_cycles);
    
    printf("First 10 points:\n");
    for (int i = 0; i < 10; i++) {
        printf("[%d] %.4f\n", i, duty_cycles[i]);
    }
    
    printf("\nLast 10 points:\n");
    for (int i = 190; i < 200; i++) {
        printf("[%d] %.4f\n", i, duty_cycles[i]);
    }
    
    if (validate_duty_cycles(duty_cycles, config.samples_per_cycle)) {
        printf("\nAll duty cycle values are within valid range!\n");
    } else {
        printf("\nWarning: Invalid duty cycle values found!\n");
    }
    
    return 0;
}