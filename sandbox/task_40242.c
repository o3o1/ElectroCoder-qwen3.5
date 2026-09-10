#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef enum {
    ACTIVE,
    SLEEP
} power_mode_t;

typedef struct {
    power_mode_t current_mode;
    float output_current;  // mA
    float sleep_threshold; // mA
    int wakeup_counter;
} dc_dc_converter_t;

void init_converter(dc_dc_converter_t *conv, float sleep_threshold) {
    conv->current_mode = ACTIVE;
    conv->output_current = 0.0f;
    conv->sleep_threshold = sleep_threshold;
    conv->wakeup_counter = 0;
}

void update_current_measurement(dc_dc_converter_t *conv, float new_current) {
    conv->output_current = new_current;
    
    if (conv->current_mode == ACTIVE) {
        if (new_current < conv->sleep_threshold) {
            conv->wakeup_counter++;
            if (conv->wakeup_counter >= 3) {
                conv->current_mode = SLEEP;
                conv->wakeup_counter = 0;
                printf("状态变更: ACTIVE -> SLEEP (电流: %.1f mA < 阈值: %.1f mA)\n", 
                       new_current, conv->sleep_threshold);
            }
        } else {
            conv->wakeup_counter = 0;
        }
    } else { // SLEEP mode
        if (new_current > conv->sleep_threshold) {
            conv->current_mode = ACTIVE;
            conv->wakeup_counter = 0;
            printf("状态变更: SLEEP -> ACTIVE (电流: %.1f mA > 阈值: %.1f mA)\n", 
                   new_current, conv->sleep_threshold);
        }
    }
}

int main() {
    dc_dc_converter_t converter;
    float current_sequence[] = {55.0f, 48.0f, 45.0f, 42.0f, 60.0f};
    int num_cycles = sizeof(current_sequence) / sizeof(current_sequence[0]);
    
    init_converter(&converter, 50.0f);
    
    printf("DC/DC变换器休眠唤醒控制系统\n");
    printf("休眠阈值: %.1f mA\n\n", converter.sleep_threshold);
    
    for (int i = 0; i < num_cycles; i++) {
        printf("周期 %d: 电流 = %.1f mA, ", i + 1, current_sequence[i]);
        update_current_measurement(&converter, current_sequence[i]);
        printf("当前模式: %s\n", converter.current_mode == ACTIVE ? "ACTIVE" : "SLEEP");
    }
    
    printf("\n最终状态总结:\n");
    printf("模式: %s\n", converter.current_mode == ACTIVE ? "ACTIVE" : "SLEEP");
    printf("最后电流: %.1f mA\n", converter.output_current);
    printf("唤醒计数器: %d\n", converter.wakeup_counter);
    
    return 0;
}