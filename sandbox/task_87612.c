#include <stdio.h>
#include <math.h>

#define PWM_PERIOD 10  // 模拟PWM周期点数

float calculate_zvs_delay(float resonant_inductance, float parasitic_capacitance, float load_current, float bus_voltage) {
    float Lr = resonant_inductance * 1e-6f;  // μH -> H
    float Cr = parasitic_capacitance * 1e-9f; // nF -> F
    
    float resonant_time = (M_PI / 2.0f) * sqrtf(Lr * Cr);
    float current_commutation_time = (2.0f * Lr * load_current) / bus_voltage;
    
    float delay_seconds = resonant_time - current_commutation_time;
    
    if (delay_seconds < 0) {
        delay_seconds = 0;
    }
    
    return delay_seconds * 1e6f;  // 秒 -> 微秒
}

void generate_pwm_signals(float zvs_delay_us, int q1_state[], int q2_state[], int q3_state[], int q4_state[]) {
    float period_us = 20.0f;  // 假设PWM周期为20μs
    float time_per_point = period_us / PWM_PERIOD;
    
    int delay_points = (int)(zvs_delay_us / time_per_point + 0.5f);
    if (delay_points < 0) delay_points = 0;
    if (delay_points > PWM_PERIOD) delay_points = PWM_PERIOD;
    
    for (int i = 0; i < PWM_PERIOD; i++) {
        int phase = i % (PWM_PERIOD / 2);
        
        q1_state[i] = (phase < PWM_PERIOD / 4) ? 1 : 0;
        q2_state[i] = (phase >= PWM_PERIOD / 4) ? 1 : 0;
        
        int delayed_phase = (i - delay_points + PWM_PERIOD) % PWM_PERIOD;
        int delayed_mod_phase = delayed_phase % (PWM_PERIOD / 2);
        
        q3_state[i] = (delayed_mod_phase < PWM_PERIOD / 4) ? 1 : 0;
        q4_state[i] = (delayed_mod_phase >= PWM_PERIOD / 4) ? 1 : 0;
    }
}

int main() {
    float Lr = 10.0f;      // 10μH
    float Cr = 5.0f;       // 5nF
    float I_load = 5.0f;   // 5A
    float V_bus = 400.0f;  // 400V
    
    float zvs_delay = calculate_zvs_delay(Lr, Cr, I_load, V_bus);
    
    printf("ZVS延迟时间: %.2f μs\n", zvs_delay);
    
    int q1_state[PWM_PERIOD];
    int q2_state[PWM_PERIOD];
    int q3_state[PWM_PERIOD];
    int q4_state[PWM_PERIOD];
    
    generate_pwm_signals(zvs_delay, q1_state, q2_state, q3_state, q4_state);
    
    printf("Q1状态: [");
    for (int i = 0; i < PWM_PERIOD; i++) {
        printf("%d", q1_state[i]);
        if (i < PWM_PERIOD - 1) printf(",");
    }
    printf("]\n");
    
    printf("Q2状态: [");
    for (int i = 0; i < PWM_PERIOD; i++) {
        printf("%d", q2_state[i]);
        if (i < PWM_PERIOD - 1) printf(",");
    }
    printf("]\n");
    
    printf("Q3状态: [");
    for (int i = 0; i < PWM_PERIOD; i++) {
        printf("%d", q3_state[i]);
        if (i < PWM_PERIOD - 1) printf(",");
    }
    printf("]\n");
    
    printf("Q4状态: [");
    for (int i = 0; i < PWM_PERIOD; i++) {
        printf("%d", q4_state[i]);
        if (i < PWM_PERIOD - 1) printf(",");
    }
    printf("]\n");
    
    return 0;
}