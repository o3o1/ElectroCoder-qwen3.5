#include <stdio.h>
#include <math.h>
#include <stdint.h>

#define PI 3.14159265358979323846
#define SAMPLE_FREQ 10000.0f
#define GRID_FREQ 50.0f
#define SAMPLE_TIME (1.0f/SAMPLE_FREQ)
#define SOGI_K 0.707f
#define PI_KP 100.0f
#define PI_KI 2000.0f

typedef struct {
    float v_alpha;      // SOGI output: in-phase signal
    float v_beta;       // SOGI output: quadrature signal
    float v_alpha_z1;   // v_alpha previous value
    float v_beta_z1;    // v_beta previous value
    float vd;           // d-axis voltage
    float vq;           // q-axis voltage
    float freq;         // estimated frequency (rad/s)
    float freq_hz;      // estimated frequency (Hz)
    float theta;        // estimated phase angle (rad)
    float pi_integral;  // PI controller integral term
} PLL_State;

void pll_init(PLL_State* pll) {
    pll->v_alpha = 0.0f;
    pll->v_beta = 0.0f;
    pll->v_alpha_z1 = 0.0f;
    pll->v_beta_z1 = 0.0f;
    pll->vd = 0.0f;
    pll->vq = 0.0f;
    pll->freq = 2.0f * PI * GRID_FREQ;
    pll->freq_hz = GRID_FREQ;
    pll->theta = 0.0f;
    pll->pi_integral = 0.0f;
}

void pll_update(PLL_State* pll, float v_in) {
    float omega = pll->freq;
    float k = SOGI_K;
    float Ts = SAMPLE_TIME;
    
    float v_alpha_new = pll->v_alpha_z1 + k * omega * Ts * (v_in - pll->v_alpha_z1) - omega * Ts * pll->v_beta_z1;
    float v_beta_new = pll->v_beta_z1 + omega * Ts * pll->v_alpha_z1;
    
    pll->v_alpha = v_alpha_new;
    pll->v_beta = v_beta_new;
    pll->v_alpha_z1 = v_alpha_new;
    pll->v_beta_z1 = v_beta_new;
    
    float sin_theta = sinf(pll->theta);
    float cos_theta = cosf(pll->theta);
    
    pll->vd = pll->v_alpha * cos_theta + pll->v_beta * sin_theta;
    pll->vq = -pll->v_alpha * sin_theta + pll->v_beta * cos_theta;
    
    float error = pll->vq;
    
    pll->pi_integral += PI_KI * Ts * error;
    float freq_delta = PI_KP * error + pll->pi_integral;
    
    pll->freq = 2.0f * PI * GRID_FREQ + freq_delta;
    pll->freq_hz = pll->freq / (2.0f * PI);
    
    pll->theta += pll->freq * Ts;
    if (pll->theta > 2.0f * PI) {
        pll->theta -= 2.0f * PI;
    }
    if (pll->theta < 0) {
        pll->theta += 2.0f * PI;
    }
}

int main() {
    PLL_State pll;
    pll_init(&pll);
    
    printf("Single-phase PLL Algorithm Test - Based on SOGI\n");
    printf("Sampling frequency: %.0f Hz, Grid frequency: %.1f Hz\n", SAMPLE_FREQ, GRID_FREQ);
    printf("Time(s)\tInput Voltage\tEstimated Freq(Hz)\tEstimated Phase(rad)\n");
    printf("------------------------------------------------------------\n");
    
    float time = 0.0f;
    float final_freq_error = 0.0f;
    float final_phase_error = 0.0f;
    
    for (int i = 0; i < 1000; i++) {
        float v_in = sinf(2.0f * PI * GRID_FREQ * time);
        
        pll_update(&pll, v_in);
        
        if (i % 100 == 0) {
            printf("%.4f\t%.6f\t%.6f\t%.6f\n", 
                   time, v_in, pll.freq_hz, pll.theta);
        }
        
        time += SAMPLE_TIME;
        
        if (i == 999) {
            float expected_phase = fmodf(2.0f * PI * GRID_FREQ * time, 2.0f * PI);
            final_freq_error = pll.freq_hz - GRID_FREQ;
            final_phase_error = pll.theta - expected_phase;
            if (final_phase_error > PI) final_phase_error -= 2.0f * PI;
            if (final_phase_error < -PI) final_phase_error += 2.0f * PI;
        }
    }
    
    printf("\nFinal Results:\n");
    printf("Estimated frequency: %.6f Hz\n", pll.freq_hz);
    printf("Frequency error: %.6f Hz\n", final_freq_error);
    printf("Phase error: %.6f rad (%.6f degrees)\n", 
           final_phase_error, final_phase_error * 180.0f / PI);
    
    return 0;
}