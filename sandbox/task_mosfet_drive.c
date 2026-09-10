#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void generate_drive_signals(int pwm_duty, int dead_time_ns, int freq_khz, int *high_side, int *low_side, int samples) {
    double period_us = 1000.0 / freq_khz;
    double period_ns = period_us * 1000.0;
    double duty_ratio = pwm_duty / 100.0;
    double high_on_time_ns = period_ns * duty_ratio;
    double dead_time_ns_double = dead_time_ns;
    
    for (int i = 0; i < samples; i++) {
        double time_ns = (i * period_ns) / samples;
        
        double high_on_start = dead_time_ns_double / 2;
        double high_on_end = high_on_time_ns - dead_time_ns_double / 2;
        double low_on_start = high_on_time_ns + dead_time_ns_double / 2;
        double low_on_end = period_ns - dead_time_ns_double / 2;
        
        if (time_ns >= high_on_start && time_ns < high_on_end) {
            high_side[i] = 1;
        } else {
            high_side[i] = 0;
        }
        
        if (time_ns >= low_on_start && time_ns < low_on_end) {
            low_side[i] = 1;
        } else {
            low_side[i] = 0;
        }
    }
}

void simulate_miller_plateau(int *gate_voltage, int samples, int vth, int vgp, int miller_time_samples) {
    int plateau_started = 0;
    int plateau_counter = 0;
    
    for (int i = 0; i < samples; i++) {
        if (!plateau_started && gate_voltage[i] >= vth) {
            plateau_started = 1;
        }
        
        if (plateau_started) {
            if (plateau_counter < miller_time_samples) {
                gate_voltage[i] = vgp;
                plateau_counter++;
            } else {
                break;
            }
        }
    }
}

int main() {
    printf("=== MOSFET drive circuit simulation test ===\n\n");
    
    printf("Test 1: Dead time simulation\n");
    printf("Parameters: duty=60%%, dead time=100ns, frequency=50kHz, samples=100\n");
    
    int samples = 100;
    int *high_side = (int*)malloc(samples * sizeof(int));
    int *low_side = (int*)malloc(samples * sizeof(int));
    
    generate_drive_signals(60, 100, 50, high_side, low_side, samples);
    
    printf("First 20 sample states:\n");
    printf("Sample\tHigh\tLow\n");
    for (int i = 0; i < 20; i++) {
        printf("%d\t%d\t%d\n", i, high_side[i], low_side[i]);
    }
    
    printf("\nTest 2: Miller plateau effect simulation\n");
    printf("Parameters: vth=3V, vgp=5V, plateau duration=20 samples\n");
    
    int *gate_voltage = (int*)malloc(samples * sizeof(int));
    for (int i = 0; i < samples; i++) {
        gate_voltage[i] = (i * 10) / samples;
    }
    
    printf("Original gate voltage (first 30 points):\n");
    for (int i = 0; i < 30; i++) {
        printf("%d ", gate_voltage[i]);
    }
    printf("\n");
    
    simulate_miller_plateau(gate_voltage, samples, 3, 5, 20);
    
    printf("Gate voltage after Miller plateau simulation (first 30 points):\n");
    for (int i = 0; i < 30; i++) {
        printf("%d ", gate_voltage[i]);
    }
    printf("\n");
    
    printf("\nDetailed voltage change process:\n");
    printf("Sample\tVoltage(V)\n");
    for (int i = 0; i < 30; i++) {
        printf("%d\t%d\n", i, gate_voltage[i]);
    }
    
    free(high_side);
    free(low_side);
    free(gate_voltage);
    
    printf("\n=== Test completed ===\n");
    
    return 0;
}