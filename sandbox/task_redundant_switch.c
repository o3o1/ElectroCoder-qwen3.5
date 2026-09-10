#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#define MODULE_A_STATUS   (*((volatile uint8_t*)0x40001000))
#define MODULE_A_CONTROL  (*((volatile uint8_t*)0x40001001))
#define MODULE_A_CURRENT  (*((volatile uint16_t*)0x40001002))
#define MODULE_B_STATUS   (*((volatile uint8_t*)0x40002000))
#define MODULE_B_CONTROL  (*((volatile uint8_t*)0x40002001))
#define MODULE_B_CURRENT  (*((volatile uint16_t*)0x40002002))

#define STATUS_WORKING      0x01
#define STATUS_READY        0x02
#define STATUS_FAULT        0x03
#define STATUS_MAINTENANCE  0x04

#define CMD_START_WORK      0x10
#define CMD_ENTER_MAINT     0x20

#define MAIN_CURRENT        100
#define BACKUP_CURRENT      100

typedef struct {
    volatile uint8_t* status_reg;
    volatile uint8_t* control_reg;
    volatile uint16_t* current_reg;
    uint8_t is_primary;
} PowerModule;

PowerModule module_a = {&MODULE_A_STATUS, &MODULE_A_CONTROL, &MODULE_A_CURRENT, 1};
PowerModule module_b = {&MODULE_B_STATUS, &MODULE_B_CONTROL, &MODULE_B_CURRENT, 0};

void init_modules(void) {
    *module_a.status_reg = STATUS_WORKING;
    *module_a.control_reg = CMD_START_WORK;
    *module_a.current_reg = MAIN_CURRENT;
    
    *module_b.status_reg = STATUS_READY;
    *module_b.control_reg = 0x00;
    *module_b.current_reg = BACKUP_CURRENT;
}

void print_status(const char* name, uint8_t status) {
    const char* status_str;
    switch(status) {
        case STATUS_WORKING: status_str = "WORKING"; break;
        case STATUS_READY: status_str = "READY"; break;
        case STATUS_FAULT: status_str = "FAULT"; break;
        case STATUS_MAINTENANCE: status_str = "MAINTENANCE"; break;
        default: status_str = "UNKNOWN"; break;
    }
    printf("%s Status: %s (0x%02X)\n", name, status_str, status);
}

uint16_t calculate_output_current(float decay_factor, float enhance_factor) {
    uint16_t main_current = *module_a.current_reg;
    uint16_t backup_current = *module_b.current_reg;
    
    return (uint16_t)(main_current * decay_factor + backup_current * enhance_factor);
}

void perform_redundant_switch(void) {
    printf("\n=== 开始冗余切换 ===\n");
    
    float decay_factor = 1.0;
    float enhance_factor = 0.0;
    float step = 0.25;
    
    for(int i = 0; i <= 4; i++) {
        uint16_t output_current = calculate_output_current(decay_factor, enhance_factor);
        printf("切换阶段 %d: 衰减系数=%.2f, 增强系数=%.2f, 输出电流=%uA\n", 
               i, decay_factor, enhance_factor, output_current);
        
        if(i < 4) {
            decay_factor -= step;
            enhance_factor += step;
            usleep(200000);
        }
    }
    
    *module_a.status_reg = STATUS_MAINTENANCE;
    *module_a.control_reg = CMD_ENTER_MAINT;
    
    *module_b.status_reg = STATUS_WORKING;
    *module_b.control_reg = CMD_START_WORK;
    
    module_a.is_primary = 0;
    module_b.is_primary = 1;
    
    printf("切换完成: Module_A进入维修状态, Module_B成为主模块\n");
}

void check_and_handle_fault(void) {
    if(*module_a.status_reg == STATUS_FAULT && module_a.is_primary) {
        printf("\n检测到主模块故障! 触发冗余切换...\n");
        perform_redundant_switch();
    }
}

int main(void) {
    printf("=== 电力电子冗余模块切换系统 ===\n\n");
    
    init_modules();
    
    printf("1. 系统初始化完成:\n");
    print_status("Module_A", *module_a.status_reg);
    print_status("Module_B", *module_b.status_reg);
    printf("Module_A电流: %uA\n", *module_a.current_reg);
    printf("Module_B电流: %uA\n", *module_b.current_reg);
    
    printf("\n2. 主模块正常工作状态:\n");
    uint16_t initial_output = calculate_output_current(1.0, 0.0);
    printf("输出电流 = 主模块电流 = %uA\n", initial_output);
    
    printf("\n3. 模拟主模块故障...\n");
    *module_a.status_reg = STATUS_FAULT;
    print_status("Module_A", *module_a.status_reg);
    
    check_and_handle_fault();
    
    printf("\n4. 最终状态:\n");
    print_status("Module_A", *module_a.status_reg);
    print_status("Module_B", *module_b.status_reg);
    printf("Module_A电流: %uA\n", *module_a.current_reg);
    printf("Module_B电流: %uA\n", *module_b.current_reg);
    uint16_t final_output = calculate_output_current(0.0, 1.0);
    printf("最终输出电流 = 新主模块电流 = %uA\n", final_output);
    
    return 0;
}