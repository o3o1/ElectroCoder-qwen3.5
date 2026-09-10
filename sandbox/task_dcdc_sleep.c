#include <stdio.h>
#include <stdint.h>

// 模拟硬件寄存器宏定义
#define LOAD_CURRENT_REG   (*((volatile uint32_t*)0x40000000))  // 模拟负载电流寄存器
#define CONTROL_REG        (*((volatile uint32_t*)0x40000004))  // 模拟控制寄存器

#define SLEEP_THRESHOLD    15   // 休眠阈值(mA)
#define WAKE_THRESHOLD     25   // 唤醒阈值(mA)

// 状态定义
typedef enum {
    STATE_ACTIVE = 0,
    STATE_SLEEP = 1
} dcdc_state_t;

// 全局状态变量
static dcdc_state_t current_state = STATE_ACTIVE;
static int transition_count = 0;

// 模拟寄存器操作函数
static void mock_set_load_current(int current) {
    // 模拟设置负载电流寄存器
    volatile uint32_t* reg = (volatile uint32_t*)0x40000000;
    *reg = (uint32_t)current;
}

static int mock_get_load_current(void) {
    // 模拟读取负载电流寄存器
    volatile uint32_t* reg = (volatile uint32_t*)0x40000000;
    return (int)(*reg);
}

static void mock_set_control_reg(uint32_t value) {
    // 模拟设置控制寄存器
    volatile uint32_t* reg = (volatile uint32_t*)0x40000004;
    *reg = value;
}

// DC/DC状态机函数
void dcdc_state_machine(int load_current) {
    // 设置负载电流到模拟寄存器
    mock_set_load_current(load_current);
    
    dcdc_state_t new_state = current_state;
    
    // 状态转换逻辑
    if (current_state == STATE_ACTIVE) {
        if (load_current < SLEEP_THRESHOLD) {
            new_state = STATE_SLEEP;
            mock_set_control_reg(0x01);  // 设置休眠标志
        } else {
            mock_set_control_reg(0x00);  // 保持活跃标志
        }
    } else { // STATE_SLEEP
        if (load_current > WAKE_THRESHOLD) {
            new_state = STATE_ACTIVE;
            mock_set_control_reg(0x02);  // 设置唤醒标志
        } else {
            mock_set_control_reg(0x01);  // 保持休眠标志
        }
    }
    
    // 如果状态发生变化，记录转换
    if (new_state != current_state) {
        if (current_state == STATE_ACTIVE && new_state == STATE_SLEEP) {
            printf("状态: ACTIVE -> SLEEP, 电流: %d mA\n", load_current);
        } else if (current_state == STATE_SLEEP && new_state == STATE_ACTIVE) {
            printf("状态: SLEEP -> ACTIVE, 电流: %d mA\n", load_current);
        }
        current_state = new_state;
        transition_count++;
    }
}

// 状态名称转换函数
const char* get_state_name(dcdc_state_t state) {
    return (state == STATE_ACTIVE) ? "ACTIVE" : "SLEEP";
}

int main(void) {
    printf("DC/DC变换器休眠唤醒控制模拟程序\n");
    printf("================================\n");
    
    // 初始化状态为ACTIVE
    current_state = STATE_ACTIVE;
    transition_count = 0;
    mock_set_control_reg(0x00);  // 初始化为活跃状态
    
    // 测试用例1: 从高负载到低负载再到高负载
    printf("\n测试用例1: 负载变化序列 [30, 10, 40] mA\n");
    int test_case1[] = {30, 10, 40};
    for (int i = 0; i < 3; i++) {
        dcdc_state_machine(test_case1[i]);
    }
    
    // 测试用例2: 在阈值附近波动
    printf("\n测试用例2: 负载变化序列 [20, 14, 16, 26, 24] mA\n");
    int test_case2[] = {20, 14, 16, 26, 24};
    for (int i = 0; i < 5; i++) {
        dcdc_state_machine(test_case2[i]);
    }
    
    // 测试用例3: 长时间低负载后唤醒
    printf("\n测试用例3: 负载变化序列 [5, 8, 12, 30, 35] mA\n");
    int test_case3[] = {5, 8, 12, 30, 35};
    for (int i = 0; i < 5; i++) {
        dcdc_state_machine(test_case3[i]);
    }
    
    // 打印最终结果
    printf("\n================================\n");
    printf("最终状态: %s\n", get_state_name(current_state));
    printf("总转换次数: %d\n", transition_count);
    
    return 0;
}