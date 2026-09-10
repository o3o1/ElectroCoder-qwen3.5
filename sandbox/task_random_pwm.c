#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265358979323846f

float random_triangle_wave(float t, float base_freq, float freq_range) {
    static float current_freq = 0.0f;
    static float last_cycle = -1.0f;
    
    float phase = t * base_freq;
    int cycle = (int)phase;
    
    if (cycle > last_cycle) {
        float rand_val = (float)rand() / RAND_MAX;
        current_freq = base_freq + (rand_val * 2.0f - 1.0f) * freq_range;
        last_cycle = cycle;
    }
    
    float actual_phase = t * current_freq;
    float triangle_phase = actual_phase - floorf(actual_phase);
    
    if (triangle_phase < 0.5f) {
        return -1.0f + 4.0f * triangle_phase;
    } else {
        return 1.0f - 4.0f * (triangle_phase - 0.5f);
    }
}

int random_pwm(float t, float modulation_index) {
    float carrier = random_triangle_wave(t, 1000.0f, 200.0f);
    float mod_signal = modulation_index * sinf(2.0f * PI * 50.0f * t);
    
    return (mod_signal > carrier) ? 1 : 0;
}

int main() {
    srand(0);
    
    float t_start = 0.0f;
    float t_end = 0.1f;
    float dt = 0.0001f;
    float modulation_index = 0.5f;
    float base_freq = 1000.0f;
    float freq_range = 200.0f;
    
    int total_samples = (int)((t_end - t_start) / dt) + 1;
    int high_count = 0;
    
    printf("Time(s)  Carrier  ModSignal  PWM\n");
    
    for (int i = 0; i < total_samples; i++) {
        float t = t_start + i * dt;
        
        float carrier = random_triangle_wave(t, base_freq, freq_range);
        float mod_signal = modulation_index * sinf(2.0f * PI * 50.0f * t);
        int pwm_output = (mod_signal > carrier) ? 1 : 0;
        
        if (pwm_output) {
            high_count++;
        }
        
        if (i < 5) {
            printf("%f  %f  %f  %d\n", t, carrier, mod_signal, pwm_output);
        }
    }
    
    float duty_cycle = (float)high_count / total_samples * 100.0f;
    printf("Duty cycle: %.2f%%\n", duty_cycle);
    
    return 0;
}