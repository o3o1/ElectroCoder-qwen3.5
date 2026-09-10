#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define PI 3.14159265358979323846
#define SAMPLE_RATE 10000.0f    // 10kHz sampling rate
#define GRID_FREQ 50.0f        // Grid frequency 50Hz
#define GRID_VOLTAGE 220.0f    // Grid voltage amplitude 220V
#define SAMPLE_PERIOD (1.0f/SAMPLE_RATE)
#define OMEGA_NOMINAL (2.0f*PI*GRID_FREQ)  // Nominal angular frequency

// PLL-less synchronization control structure
typedef struct {
    float theta;        // Estimated phase angle (radians)
    float freq;         // Estimated frequency (Hz)
    float u_alpha_prev; // Previous ?-axis voltage
    float u_beta_prev;  // Previous ?-axis voltage
    float integral;     // Integrator state
    float kp;           // Proportional coefficient
    float ki;           // Integral coefficient
} PLLessSync;

// Clarke transform: three-phase abc to ?? coordinate system
void clarke_transform(float ua, float ub, float uc, float *u_alpha, float *u_beta) {
    // Clarke transform matrix
    *u_alpha = (2.0f/3.0f) * (ua - 0.5f*ub - 0.5f*uc);
    *u_beta = (2.0f/3.0f) * (0.86602540378f*ub - 0.86602540378f*uc); // sqrt(3)/2 ??0.8660254
}

// PLL-less synchronization control core algorithm
void plless_sync_update(PLLessSync *sync, float ua, float ub, float uc) {
    float u_alpha, u_beta;
    
    // 1. Clarke transform
    clarke_transform(ua, ub, uc, &u_alpha, &u_beta);
    
    // 2. Calculate instantaneous reactive power q = u? * u?' - u? * u?'
    float q = u_alpha * sync->u_beta_prev - u_beta * sync->u_alpha_prev;
    
    // 3. Calculate voltage amplitude square (for normalization)
    float u_sq = u_alpha*u_alpha + u_beta*u_beta;
    
    // 4. Avoid division by zero, calculate phase error
    float phase_error = 0.0f;
    if (u_sq > 1e-6f) {
        // Phase error ??q / u_sq (small angle approximation)
        phase_error = q / u_sq;
    }
    
    // 5. PI controller processes phase error
    float delta_theta = sync->kp * phase_error + sync->ki * sync->integral;
    
    // 6. Update integrator state
    sync->integral += phase_error * SAMPLE_PERIOD;
    
    // 7. Update phase angle (integrate to get phase)
    sync->theta += (OMEGA_NOMINAL + delta_theta) * SAMPLE_PERIOD;
    
    // 8. Normalize phase angle to [-?, ?] range
    while (sync->theta > PI) sync->theta -= 2.0f*PI;
    while (sync->theta < -PI) sync->theta += 2.0f*PI;
    
    // 9. Calculate frequency (phase change rate)
    sync->freq = GRID_FREQ + delta_theta / (2.0f*PI);
    
    // 10. Save current values for next cycle
    sync->u_alpha_prev = u_alpha;
    sync->u_beta_prev = u_beta;
}

// Initialize PLL-less synchronization controller
void plless_sync_init(PLLessSync *sync) {
    sync->theta = 0.0f;
    sync->freq = GRID_FREQ;
    sync->u_alpha_prev = 0.0f;
    sync->u_beta_prev = 0.0f;
    sync->integral = 0.0f;
    sync->kp = 100.0f;  // Proportional coefficient (needs adjustment based on system)
    sync->ki = 5000.0f; // Integral coefficient (needs adjustment based on system)
}

// Generate three-phase voltage samples
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
    
    int total_samples = (int)(5.0f / GRID_FREQ * SAMPLE_RATE); // Number of samples for 5 cycles
    float time = 0.0f;
    
    float freq_sum = 0.0f;
    int valid_count = 0;
    
    printf("PLL-less Synchronization Control Algorithm Simulation\n");
    printf("Sampling Rate: %.0f Hz, Grid Frequency: %.1f Hz, Voltage Amplitude: %.1f V\n", 
           SAMPLE_RATE, GRID_FREQ, GRID_VOLTAGE);
    printf("============================================================\n");
    printf("Sample\tTime(s)\tPhase(rad)\tFrequency(Hz)\tFreq Error(Hz)\n");
    printf("============================================================\n");
    
    for (int i = 0; i < total_samples; i++) {
        float ua, ub, uc;
        
        // Generate three-phase voltage
        generate_three_phase_voltage(time, &ua, &ub, &uc);
        
        // Update synchronization control algorithm
        plless_sync_update(&sync, ua, ub, uc);
        
        // Calculate frequency error
        float freq_error = sync.freq - GRID_FREQ;
        
        // Skip first few samples (let algorithm stabilize)
        if (i > 100) {
            freq_sum += fabsf(freq_error);
            valid_count++;
        }
        
        // Print results every 100 samples
        if (i % 100 == 0) {
            printf("%d\t%.4f\t%.4f\t\t%.4f\t\t%.4f\n", 
                   i, time, sync.theta, sync.freq, freq_error);
        }
        
        time += SAMPLE_PERIOD;
    }
    
    // Calculate average frequency error
    float avg_freq_error = (valid_count > 0) ? freq_sum / valid_count : 0.0f;
    
    printf("============================================================\n");
    printf("Simulation complete! Processed %d samples (%.1f cycles)\n", 
           total_samples, (float)total_samples * GRID_FREQ / SAMPLE_RATE);
    printf("Average frequency estimation error: %.6f Hz\n", avg_freq_error);
    printf("Relative error: %.4f%%\n", avg_freq_error / GRID_FREQ * 100.0f);
    
    return 0;
}
