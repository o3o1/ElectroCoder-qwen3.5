#include <stdio.h>
#include <math.h>

#define PI 3.14159265358979323846

typedef struct {
    int phase_id;
    double duty_cycle;
    double phase_shift;
    double current_ripple;
} PhaseData;

void calculate_total_current(int N, PhaseData phases[], double *total_ripple) {
    double max_single_ripple = 0.0;
    double weighted_sum = 0.0;
    double weight_sum = 0.0;
    
    for (int i = 0; i < N; i++) {
        phases[i].current_ripple = phases[i].duty_cycle * (1.0 - phases[i].duty_cycle) * 100.0;
        
        if (phases[i].current_ripple > max_single_ripple) {
            max_single_ripple = phases[i].current_ripple;
        }
        
        double weight = fabs(cos(phases[i].phase_shift));
        weighted_sum += phases[i].current_ripple * weight;
        weight_sum += weight;
    }
    
    if (weight_sum > 0.0) {
        *total_ripple = weighted_sum / weight_sum;
    } else {
        *total_ripple = max_single_ripple;
    }
}

int main() {
    printf("=== 3-Phase Interleaved DC/DC Converter Ripple Suppression Simulation ===\n");
    
    PhaseData phases_3[3];
    double total_ripple_3;
    
    for (int i = 0; i < 3; i++) {
        phases_3[i].phase_id = i;
        phases_3[i].duty_cycle = 0.4;
        phases_3[i].phase_shift = 2.0 * PI * i / 3.0;
    }
    
    calculate_total_current(3, phases_3, &total_ripple_3);
    
    printf("Phase Parameters:\n");
    for (int i = 0; i < 3; i++) {
        printf("Phase %d: Duty Cycle=%.2f, Phase Shift=%.2f rad, Single-Phase Ripple=%.2f\n",
               phases_3[i].phase_id, phases_3[i].duty_cycle, 
               phases_3[i].phase_shift, phases_3[i].current_ripple);
    }
    printf("Total Output Current Ripple: %.2f\n\n", total_ripple_3);
    
    printf("=== 4-Phase Interleaved DC/DC Converter Ripple Suppression Simulation ===\n");
    
    PhaseData phases_4[4];
    double total_ripple_4;
    
    for (int i = 0; i < 4; i++) {
        phases_4[i].phase_id = i;
        phases_4[i].duty_cycle = 0.4;
        phases_4[i].phase_shift = 2.0 * PI * i / 4.0;
    }
    
    calculate_total_current(4, phases_4, &total_ripple_4);
    
    printf("Phase Parameters:\n");
    for (int i = 0; i < 4; i++) {
        printf("Phase %d: Duty Cycle=%.2f, Phase Shift=%.2f rad, Single-Phase Ripple=%.2f\n",
               phases_4[i].phase_id, phases_4[i].duty_cycle, 
               phases_4[i].phase_shift, phases_4[i].current_ripple);
    }
    printf("Total Output Current Ripple: %.2f\n\n", total_ripple_4);
    
    printf("=== Comparison Analysis ===\n");
    printf("3-Phase Total Ripple: %.2f\n", total_ripple_3);
    printf("4-Phase Total Ripple: %.2f\n", total_ripple_4);
    printf("Ripple Suppression Improvement: %.2f%%\n", (total_ripple_3 - total_ripple_4) / total_ripple_3 * 100.0);
    
    return 0;
}