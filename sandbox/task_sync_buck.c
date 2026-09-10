#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define T_CYCLE 100
#define DUTY_CYCLE 0.6
#define DEAD_TIME 2
#define CURRENT_THRESHOLD 0.1

#define V_GS_HIGH 0
#define V_GS_LOW 1

int g_vgs_high = 0;
int g_vgs_low = 0;
float g_i_l = 0.0;

void simulate_cycle(float current) {
    int high_on_time = (int)(T_CYCLE * DUTY_CYCLE);
    int high_off_time = T_CYCLE - high_on_time;
    
    printf("周期时序: 上管开启时间=%dμs, 关闭时间=%dμs, 死区时间=%dμs\n", 
           high_on_time, high_off_time, DEAD_TIME);
    
    for (int t = 0; t < T_CYCLE; t++) {
        if (t < high_on_time) {
            g_vgs_high = 1;
            g_vgs_low = 0;
        } else if (t < high_on_time + DEAD_TIME) {
            g_vgs_high = 0;
            if (current > CURRENT_THRESHOLD) {
                g_vgs_low = 1;
                printf("t=%dμs: 上管=0, 下管=1 [体二极管抑制激活, I_L=%.3fA>%.3fA]\n", 
                       t, current, CURRENT_THRESHOLD);
            } else {
                g_vgs_low = 0;
                printf("t=%dμs: 上管=0, 下管=0 [体二极管续流, I_L=%.3fA≤%.3fA]\n", 
                       t, current, CURRENT_THRESHOLD);
            }
        } else {
            g_vgs_high = 0;
            g_vgs_low = 1;
        }
    }
}

void test_case(float current, const char* case_name) {
    printf("\n=== 测试用例: %s (I_L = %.3fA) ===\n", case_name, current);
    simulate_cycle(current);
}

int main() {
    printf("同步整流Buck变换器体二极管导通抑制模拟\n");
    printf("========================================\n");
    
    test_case(0.5, "正向大电流");
    test_case(0.05, "正向小电流");
    test_case(-0.2, "反向电流");
    
    printf("\n模拟完成\n");
    return 0;
}