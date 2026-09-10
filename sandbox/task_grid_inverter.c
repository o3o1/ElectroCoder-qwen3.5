#include <stdio.h>
#include <stdbool.h>

typedef struct {
    float current_freq;
    float target_freq;
    bool is_locked;
    float phase_error;
} GridInverter;

void detect_grid_frequency(GridInverter *inv, float measured_freq) {
    inv->current_freq = measured_freq;
    if (measured_freq >= 45.0f && measured_freq <= 55.0f) {
        inv->is_locked = true;
    } else {
        inv->is_locked = false;
    }
}

void adaptive_control(GridInverter *inv) {
    if (!inv->is_locked) {
        inv->target_freq = 0.0f;
        return;
    }
    
    float freq = inv->current_freq;
    
    if (freq >= 49.5f && freq <= 50.5f) {
        inv->target_freq = 50.0f;
    } else if (freq >= 45.0f && freq < 49.5f) {
        inv->target_freq = 50.0f + (50.0f - freq) * 0.3f;
    } else if (freq > 50.5f && freq <= 55.0f) {
        inv->target_freq = 50.0f - (freq - 50.0f) * 0.3f;
    }
    
    if (inv->target_freq < 45.0f) inv->target_freq = 45.0f;
    if (inv->target_freq > 55.0f) inv->target_freq = 55.0f;
}

void update_phase(GridInverter *inv) {
    if (!inv->is_locked) {
        inv->phase_error = 0.0f;
        return;
    }
    inv->phase_error = (inv->target_freq - inv->current_freq) * 10.0f;
}

int main() {
    GridInverter inverter;
    
    printf("并网逆变器频率自适应控制测试\n");
    printf("==============================\n\n");
    
    float test_cases[] = {49.0f, 50.2f, 52.5f, 44.0f};
    int num_tests = sizeof(test_cases) / sizeof(test_cases[0]);
    
    for (int i = 0; i < num_tests; i++) {
        printf("测试用例 %d: 测量频率 %.1fHz\n", i+1, test_cases[i]);
        
        detect_grid_frequency(&inverter, test_cases[i]);
        adaptive_control(&inverter);
        update_phase(&inverter);
        
        printf("  当前频率: %.2f Hz\n", inverter.current_freq);
        printf("  目标频率: %.2f Hz\n", inverter.target_freq);
        printf("  锁定状态: %s\n", inverter.is_locked ? "锁定" : "未锁定");
        printf("  相位误差: %.2f 度\n", inverter.phase_error);
        printf("\n");
    }
    
    return 0;
}