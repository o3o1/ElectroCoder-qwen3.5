#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define EPSILON 0.01f
#define BATTERY_VOLTAGE_NOMINAL 12.0f
#define BATTERY_VOLTAGE_MIN 10.5f
#define BATTERY_VOLTAGE_MAX 14.5f
#define BATTERY_VOLTAGE_FLOAT 13.8f
#define BATTERY_VOLTAGE_CUTOFF 11.0f
#define BATTERY_VOLTAGE_RESTORE 11.5f
#define BATTERY_CAPACITY 7.0f
#define PWM_MAX 1.0f
#define PWM_MIN 0.0f
#define PWM_STEP 0.05f

typedef struct {
    float V_pv;
    float I_pv;
    float P_pv;
    float D;
    float V_bat;
    int charging_state;
    int load_state;
} SolarControllerState;

float calculate_pv_current(float V_pv, float k) {
    if (V_pv < 0 || V_pv > 24.0f) return 0.0f;
    return k * V_pv * (1.0f - V_pv / 24.0f);
}

void mppt_incremental_conductance(SolarControllerState* state, SolarControllerState* prev_state, float k) {
    float delta_P = state->P_pv - prev_state->P_pv;
    float delta_V = state->V_pv - prev_state->V_pv;
    
    if (fabsf(delta_V) < EPSILON) {
        if (delta_P > 0) {
            state->D += PWM_STEP;
        } else {
            state->D -= PWM_STEP;
        }
    } else {
        float dP_dV = delta_P / delta_V;
        float P_V_ratio = -state->P_pv / state->V_pv;
        
        if (dP_dV > P_V_ratio) {
            state->D += PWM_STEP;
        } else {
            state->D -= PWM_STEP;
        }
    }
    
    if (state->D > PWM_MAX) state->D = PWM_MAX;
    if (state->D < PWM_MIN) state->D = PWM_MIN;
    
    state->V_pv = 24.0f * state->D;
    state->I_pv = calculate_pv_current(state->V_pv, k);
    state->P_pv = state->V_pv * state->I_pv;
}

void update_battery_state(SolarControllerState* state, float delta_time) {
    float charge_current = 0.0f;
    float load_current = 0.0f;
    
    if (state->P_pv > 0 && state->V_bat < BATTERY_VOLTAGE_MAX) {
        if (state->V_bat < BATTERY_VOLTAGE_FLOAT) {
            charge_current = 1.0f;
            state->charging_state = 1;
        } else {
            charge_current = 0.1f;
            state->charging_state = 2;
        }
    } else {
        state->charging_state = 0;
    }
    
    if (state->V_bat < BATTERY_VOLTAGE_CUTOFF) {
        state->load_state = 0;
        load_current = 0.0f;
    } else if (state->V_bat >= BATTERY_VOLTAGE_RESTORE) {
        state->load_state = 1;
        load_current = 0.5f;
    }
    
    float net_current = charge_current - load_current;
    float delta_V = net_current * delta_time / BATTERY_CAPACITY;
    state->V_bat += delta_V;
    
    if (state->V_bat < BATTERY_VOLTAGE_MIN) state->V_bat = BATTERY_VOLTAGE_MIN;
    if (state->V_bat > BATTERY_VOLTAGE_MAX) state->V_bat = BATTERY_VOLTAGE_MAX;
}

void print_state(int iteration, SolarControllerState* state, float k) {
    printf("光照k=%.1f, 迭代%d: V_pv=%.2fV, I_pv=%.2fA, P_pv=%.2fW, D=%.2f, V_bat=%.2fV, ", 
           k, iteration, state->V_pv, state->I_pv, state->P_pv, state->D, state->V_bat);
    
    switch(state->charging_state) {
        case 0: printf("充电:关, "); break;
        case 1: printf("充电:恒流, "); break;
        case 2: printf("充电:浮充, "); break;
    }
    
    printf("负载:%s\n", state->load_state ? "开" : "关");
}

int main() {
    float k_values[] = {0.3f, 0.7f, 0.9f, 0.2f};
    int num_k_values = sizeof(k_values) / sizeof(k_values[0]);
    int iterations_per_k = 5;
    
    SolarControllerState current_state = {0};
    SolarControllerState prev_state = {0};
    
    current_state.V_bat = 12.0f;
    current_state.D = 0.5f;
    current_state.charging_state = 0;
    current_state.load_state = 1;
    
    printf("=== 太阳能路灯控制器模拟 ===\n");
    
    for (int k_idx = 0; k_idx < num_k_values; k_idx++) {
        float k = k_values[k_idx];
        printf("\n--- 光照强度 k=%.1f ---\n", k);
        
        current_state.V_pv = 24.0f * current_state.D;
        current_state.I_pv = calculate_pv_current(current_state.V_pv, k);
        current_state.P_pv = current_state.V_pv * current_state.I_pv;
        
        for (int iter = 0; iter < iterations_per_k; iter++) {
            prev_state = current_state;
            
            if (iter > 0) {
                mppt_incremental_conductance(&current_state, &prev_state, k);
            }
            
            update_battery_state(&current_state, 1.0f);
            print_state(iter + 1, &current_state, k);
        }
    }
    
    printf("\n=== 电池状态总结 ===\n");
    printf("最终电池电压: %.2fV\n", current_state.V_bat);
    printf("充电状态: ");
    switch(current_state.charging_state) {
        case 0: printf("未充电\n"); break;
        case 1: printf("恒流充电\n"); break;
        case 2: printf("浮充\n"); break;
    }
    printf("负载状态: %s\n", current_state.load_state ? "开启" : "关闭");
    
    return 0;
}