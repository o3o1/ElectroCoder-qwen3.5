#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define PI 3.14159265358979323846
#define SAMPLE_RATE 10000.0f
#define GRID_FREQ 50.0f
#define GRID_VOLTAGE 220.0f
#define SAMPLE_PERIOD (1.0f/SAMPLE_RATE)
#define OMEGA_NOMINAL (2.0f*PI*GRID_FREQ)

typedef struct {
    float theta;
    float freq;
    float u_alpha_prev;
    float u_beta_prev;
    float integral;
    float kp;
    float ki;
} PLLessSync;

void clarke_transform(float ua, float ub, float uc, float *u_alpha, float *u_beta) {
    *u_alpha = (2.0f/3.0f) * (ua - 0.5f*ub - 0.5f*uc);
    *u_beta = (2.0f/3.0f) * (0.86602540378f*ub - 0.86602540378f*uc);
}

void plless_sync_update(PLLessSync *sync, float ua, float ub, float uc) {
    float u_alpha, u_beta;
    
    clarke_transform(ua, ub, uc, &u_alpha, &u_beta);
    
    float q = u_alpha * sync->u_beta_prev - u_beta * sync->u_alpha_prev;
    
    float u_sq = u_alpha*u_alpha + u_beta*u_beta;
    
    float phase_error = 0.0f;
    if (u_sq > 1e-6f) {
        phase_error = q / u_sq;
    }
    
    float delta_theta = sync->kp * phase_error + sync->ki * sync->integral;
    
    sync->integral += phase_error * SAMPLE_PERIOD;
    
    sync->theta += (OMEGA_NOMINAL + delta_theta) * SAMPLE_PERIOD;
    
    while (sync->theta > PI) sync->theta -= 2.0f*PI;
    while (sync->theta < -PI) sync->theta += 2.0f*PI;
    
    sync->freq = GRID_FREQ + delta_theta / (2.0f*PI);
    
    sync->u_alpha_prev = u_alpha;
    sync->u_beta_prev = u_beta;
}

void plless_sync_init(PLLessSync *sync) {
    sync->theta = 0.0f;
    sync->freq = GRID_FREQ;
    sync->u_alpha_prev = 0.0f;
    sync->u_beta_prev = 0.0f;
    sync->integral = 0.0f;
    sync->kp = 100.0f;
    sync->ki = 5000.0f;
}

void generate_three_phase_voltage(float time, float *ua, float *ub, float *uc) {
    float omega = 2.0f * PI * GRID_FREQ;
    float phase = omega * time;
    
    *ua = GRID_VOLTAGE * sinf(phase);
    *ub = GRID_VOLTAGE * sinf(phase - 2.0f*PI/3.0f);
    *uc = GRID_VOLTAGE * sinf(phase + 2.0f*PI/3.0f);
}

int main() {
    PLLessSync sync;
    plless_sync_init(&sync);
    
    int total_samples = (int)(5.0f / GRID_FREQ * SAMPLE_RATE);
    float time = 0.0f;
    
    float freq_sum = 0.0f;
    int valid_count = 0;
    
    printf("PLL-less Sync Control Simulation\n");
    printf("Sampling Rate: %.0f Hz, Grid Freq: %.1f Hz, Voltage: %.1f V\n", 
           SAMPLE_RATE, GRID_FREQ, GRID_VOLTAGE);
    printf("============================================================\n");
    printf("Sample\tTime(s)\tPhase(rad)\tFreq(Hz)\tFreqError(Hz)\n");
    printf("============================================================\n");
    
    for (int i = 0; i < total_samples; i++) {
        float ua, ub, uc;
        
        generate_three_phase_voltage(time, &ua, &ub, &uc);
        
        plless_sync_update(&sync, ua, ub, uc);
        
        float freq_error = sync.freq - GRID_FREQ;
        
        if (i > 100) {
            freq_sum += fabsf(freq_error);
            valid_count++;
        }
        
        if (i % 100 == 0) {
            printf("%d\t%.4f\t%.4f\t\t%.4f\t\t%.4f\n", 
                   i, time, sync.theta, sync.freq, freq_error);
        }
        
        time += SAMPLE_PERIOD;
    }
    
    float avg_freq_error = (valid_count > 0) ? freq_sum / valid_count : 0.0f;
    
    printf("============================================================\n");
    printf("Simulation complete! Processed %d samples (%.1f cycles)\n", 
           total_samples, (float)total_samples * GRID_FREQ / SAMPLE_RATE);
    printf("Average frequency error: %.6f Hz\n", avg_freq_error);
    printf("Relative error: %.4f%%\n", avg_freq_error / GRID_FREQ * 100.0f);
    
    return 0;
}
