#include <stdio.h>
#include <stdbool.h>

#define GRID_FREQ_NOMINAL 50.0
#define FREQ_UPPER_LIMIT 50.5
#define FREQ_LOWER_LIMIT 49.5
#define AFD_PERTURBATION 0.02
#define MAX_CYCLES 10

float detect_islanding(float measured_freq) {
    float perturbed_freq;
    
    if (measured_freq > GRID_FREQ_NOMINAL) {
        perturbed_freq = measured_freq - AFD_PERTURBATION;
    } else {
        perturbed_freq = measured_freq + AFD_PERTURBATION;
    }
    
    return perturbed_freq;
}

void test_grid_normal() {
    printf("=== Test 1: Grid Normal Condition ===\n");
    float current_freq = 50.0;
    
    for (int i = 0; i < MAX_CYCLES; i++) {
        float perturbed = detect_islanding(current_freq);
        printf("Cycle %d: Measured freq=%.3fHz, Perturbed freq=%.3fHz\n", 
               i+1, current_freq, perturbed);
        current_freq = 50.0;
    }
    printf("Grid normal: Frequency stable around nominal value\n\n");
}

void test_islanding_scenario() {
    printf("=== Test 2: Islanding Scenario ===\n");
    float current_freq = 50.0;
    bool island_detected = false;
    
    for (int cycle = 0; cycle < MAX_CYCLES; cycle++) {
        float perturbed = detect_islanding(current_freq);
        
        printf("Cycle %d: Measured freq=%.3fHz, Perturbed freq=%.3fHz", 
               cycle+1, current_freq, perturbed);
        
        if (perturbed > FREQ_UPPER_LIMIT || perturbed < FREQ_LOWER_LIMIT) {
            printf(" -> Exceeds threshold!\n");
            island_detected = true;
            break;
        } else {
            printf(" -> Within normal range\n");
        }
        
        current_freq += 0.1;
    }
    
    if (island_detected) {
        printf("Result: Islanding detection triggered\n");
    } else {
        printf("Result: Not triggered (reached max detection cycles)\n");
    }
}

int main() {
    test_grid_normal();
    test_islanding_scenario();
    return 0;
}