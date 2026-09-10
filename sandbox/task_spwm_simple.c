#include <stdio.h>
#include <stdint.h>

#define SIN_TABLE_SIZE 256
#define PWM_PERIOD 1000

static uint16_t sin_table[SIN_TABLE_SIZE];

static void init_sin_table(void) {
    for (int i = 0; i < SIN_TABLE_SIZE; i++) {
        double angle = 6.283185307179586 * i / SIN_TABLE_SIZE;
        double sin_val;
        
        if (i < SIN_TABLE_SIZE/4) {
            sin_val = (i * 1.0) / (SIN_TABLE_SIZE/4);
        } else if (i < SIN_TABLE_SIZE/2) {
            sin_val = 1.0 - ((i - SIN_TABLE_SIZE/4) * 1.0) / (SIN_TABLE_SIZE/4);
        } else if (i < 3*SIN_TABLE_SIZE/4) {
            sin_val = -((i - SIN_TABLE_SIZE/2) * 1.0) / (SIN_TABLE_SIZE/4);
        } else {
            sin_val = -1.0 + ((i - 3*SIN_TABLE_SIZE/4) * 1.0) / (SIN_TABLE_SIZE/4);
        }
        
        sin_table[i] = (uint16_t)((sin_val + 1.0) * 32767.5);
    }
}

static uint16_t get_sin_value(uint32_t phase_angle) {
    uint32_t index = (phase_angle >> 8) & 0xFF;
    return sin_table[index];
}

void generate_spwm(uint32_t phase_angle, uint32_t amplitude, uint32_t *pwm_high_ticks) {
    uint16_t sin_val = get_sin_value(phase_angle);
    
    uint64_t temp = (uint64_t)sin_val * amplitude;
    temp = temp / 32767;
    
    uint32_t result = (uint32_t)temp + 32767;
    
    uint64_t pwm_calc = (uint64_t)result * 500;
    uint32_t pwm_ticks = (uint32_t)((pwm_calc + 16384) / 32767);
    
    if (pwm_ticks > PWM_PERIOD) {
        pwm_ticks = PWM_PERIOD;
    }
    
    *pwm_high_ticks = pwm_ticks;
}

int main(void) {
    init_sin_table();
    
    uint32_t test_cases[][2] = {
        {0, 0},
        {16384, 16384},
        {32768, 32767},
        {49152, 8192}
    };
    
    for (int i = 0; i < 4; i++) {
        uint32_t phase = test_cases[i][0];
        uint32_t amplitude = test_cases[i][1];
        uint32_t pwm_ticks;
        
        generate_spwm(phase, amplitude, &pwm_ticks);
        
        printf("Phase: %u, Amplitude: %u => PWM high ticks: %u\n", 
               phase, amplitude, pwm_ticks);
    }
    
    return 0;
}