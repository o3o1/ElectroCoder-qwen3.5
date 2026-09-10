#include <stdio.h>
#include <math.h>

#define SIN_TABLE_SIZE 256
#define CARRIER_FREQ_HZ 10000.0f
#define CARRIER_PERIOD_US (1000000.0f / CARRIER_FREQ_HZ)

float sin_table[SIN_TABLE_SIZE];

void init_sin_table() {
    for (int i = 0; i < SIN_TABLE_SIZE; i++) {
        float angle = 2.0f * 3.14159265358979323846f * i / SIN_TABLE_SIZE;
        sin_table[i] = 0.5f * (1.0f + sinf(angle));
    }
}

void generate_spwm_pulses(float modulation_index, float frequency_hz, int cycles) {
    if (modulation_index < 0.0f || modulation_index > 1.0f) return;
    if (frequency_hz <= 0.0f) return;
    if (cycles <= 0) return;
    
    float sin_period_us = 1000000.0f / frequency_hz;
    float sin_step_us = sin_period_us / SIN_TABLE_SIZE;
    
    int total_steps = cycles * SIN_TABLE_SIZE;
    int last_state = 0;
    
    for (int step = 0; step < total_steps; step++) {
        int table_index = step % SIN_TABLE_SIZE;
        float sin_value = sin_table[table_index];
        float modulated_value = sin_value * modulation_index;
        
        float step_start_time = step * sin_step_us;
        float step_end_time = (step + 1) * sin_step_us;
        
        int carrier_cycles = (int)((step_end_time - step_start_time) / CARRIER_PERIOD_US);
        if (carrier_cycles < 1) carrier_cycles = 1;
        
        float carrier_step_us = (step_end_time - step_start_time) / carrier_cycles;
        
        for (int carrier = 0; carrier < carrier_cycles; carrier++) {
            float carrier_start = step_start_time + carrier * carrier_step_us;
            float carrier_mid = carrier_start + carrier_step_us / 2.0f;
            
            int new_state = (modulated_value > 0.5f) ? 1 : 0;
            
            if (new_state != last_state) {
                printf("%.2f: %d\n", carrier_mid, new_state);
                last_state = new_state;
            }
        }
    }
    
    if (last_state == 1) {
        printf("%.2f: 0\n", total_steps * sin_step_us);
    }
}

int main() {
    init_sin_table();
    
    printf("Test 1: MI=0.5, Freq=50Hz, 2 cycles\n");
    generate_spwm_pulses(0.5f, 50.0f, 2);
    
    printf("\nTest 2: MI=0.8, Freq=60Hz, 1 cycle\n");
    generate_spwm_pulses(0.8f, 60.0f, 1);
    
    printf("\nTest 3: MI=1.0, Freq=100Hz, 1 cycle\n");
    generate_spwm_pulses(1.0f, 100.0f, 1);
    
    return 0;
}