#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define PI 3.14159265358979323846
#define SAMPLE_COUNT 10
#define SAMPLE_FREQ 1000
#define SAMPLE_INTERVAL 0.001
#define VOLTAGE_PEAK 311.0
#define CURRENT 5.0
#define PULSE_THRESHOLD 0.1

double calculate_rms(double samples[], int n) {
    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        sum += samples[i] * samples[i];
    }
    return sqrt(sum / n);
}

void generate_balanced_samples(double phase_a[], double phase_b[], double phase_c[]) {
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        double angle = 2 * PI * i / SAMPLE_COUNT;
        phase_a[i] = VOLTAGE_PEAK * sin(angle);
        phase_b[i] = VOLTAGE_PEAK * sin(angle - 2 * PI / 3);
        phase_c[i] = VOLTAGE_PEAK * sin(angle + 2 * PI / 3);
    }
}

void generate_unbalanced_samples(double phase_a[], double phase_b[], double phase_c[]) {
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        double angle = 2 * PI * i / SAMPLE_COUNT;
        phase_a[i] = 1.136 * VOLTAGE_PEAK * sin(angle);
        phase_b[i] = VOLTAGE_PEAK * sin(angle - 2 * PI / 3);
        phase_c[i] = VOLTAGE_PEAK * sin(angle + 2 * PI / 3);
    }
}

void simulate_energy_accumulation(double ua_rms, double ub_rms, double uc_rms, int test_case) {
    double total_energy = 0.0;
    double energy_since_last_pulse = 0.0;
    int pulse_count = 0;
    
    printf("Test case %d - Energy accumulation simulation:\n", test_case);
    
    while (total_energy < 0.5) {
        double power = (ua_rms + ub_rms + uc_rms) * CURRENT;
        double energy_increment = power * SAMPLE_INTERVAL / 3600000.0;
        
        total_energy += energy_increment;
        energy_since_last_pulse += energy_increment;
        
        if (energy_since_last_pulse >= PULSE_THRESHOLD) {
            pulse_count++;
            printf("  Pulse event %d: Accumulated energy %.3f kWh\n", pulse_count, total_energy);
            energy_since_last_pulse = 0.0;
        }
    }
    
    printf("  Total accumulated energy: %.3f kWh, Total pulses: %d\n\n", total_energy, pulse_count);
}

int main() {
    double phase_a[SAMPLE_COUNT];
    double phase_b[SAMPLE_COUNT];
    double phase_c[SAMPLE_COUNT];
    
    printf("Smart Meter Simulation Program\n");
    printf("==============================\n\n");
    
    printf("Test case 1: Three-phase balanced condition\n");
    generate_balanced_samples(phase_a, phase_b, phase_c);
    
    double ua_rms1 = calculate_rms(phase_a, SAMPLE_COUNT);
    double ub_rms1 = calculate_rms(phase_b, SAMPLE_COUNT);
    double uc_rms1 = calculate_rms(phase_c, SAMPLE_COUNT);
    
    printf("  Phase A RMS voltage: %.2f V\n", ua_rms1);
    printf("  Phase B RMS voltage: %.2f V\n", ub_rms1);
    printf("  Phase C RMS voltage: %.2f V\n", uc_rms1);
    
    simulate_energy_accumulation(ua_rms1, ub_rms1, uc_rms1, 1);
    
    printf("Test case 2: Phase A voltage high condition\n");
    generate_unbalanced_samples(phase_a, phase_b, phase_c);
    
    double ua_rms2 = calculate_rms(phase_a, SAMPLE_COUNT);
    double ub_rms2 = calculate_rms(phase_b, SAMPLE_COUNT);
    double uc_rms2 = calculate_rms(phase_c, SAMPLE_COUNT);
    
    printf("  Phase A RMS voltage: %.2f V\n", ua_rms2);
    printf("  Phase B RMS voltage: %.2f V\n", ub_rms2);
    printf("  Phase C RMS voltage: %.2f V\n", uc_rms2);
    
    simulate_energy_accumulation(ua_rms2, ub_rms2, uc_rms2, 2);
    
    return 0;
}