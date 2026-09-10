#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int id;
    int status;
    int is_active;
    int output_value;
} Module;

void init_modules(Module* main, Module* backup) {
    main->id = 1;
    main->status = 1;
    main->is_active = 1;
    main->output_value = 100;
    
    backup->id = 2;
    backup->status = 1;
    backup->is_active = 0;
    backup->output_value = 100;
}

int get_system_output(Module* main, Module* backup) {
    if (main->is_active && main->status) {
        return main->output_value;
    } else if (backup->is_active && backup->status) {
        return backup->output_value;
    } else {
        return -1;
    }
}

void monitor_and_switch(Module* main, Module* backup) {
    if (main->is_active && !main->status && backup->status) {
        int current_output = get_system_output(main, backup);
        main->is_active = 0;
        backup->is_active = 1;
        backup->output_value = current_output;
    } else if (backup->is_active && !backup->status && main->status) {
        int current_output = get_system_output(main, backup);
        backup->is_active = 0;
        main->is_active = 1;
        main->output_value = current_output;
    }
}

void simulate_fault(Module* module, int fault_status) {
    module->status = fault_status;
}

void print_module_status(Module* module, const char* name) {
    printf("%s模块 [ID:%d]: ", name, module->id);
    printf("状态:%s, ", module->status ? "正常" : "故障");
    printf("角色:%s, ", module->is_active ? "主用" : "备用");
    printf("输出值:%d\n", module->output_value);
}

int main() {
    Module main_module, backup_module;
    
    printf("=== 电力电子冗余模块切换系统测试 ===\n\n");
    
    printf("1. 初始化系统:\n");
    init_modules(&main_module, &backup_module);
    print_module_status(&main_module, "主");
    print_module_status(&backup_module, "备");
    printf("系统输出值: %d\n\n", get_system_output(&main_module, &backup_module));
    
    printf("2. 模拟主模块故障，触发切换:\n");
    printf("切换前系统输出值: %d\n", get_system_output(&main_module, &backup_module));
    simulate_fault(&main_module, 0);
    printf("主模块故障后 - ");
    print_module_status(&main_module, "主");
    monitor_and_switch(&main_module, &backup_module);
    printf("切换后 - ");
    print_module_status(&main_module, "主");
    print_module_status(&backup_module, "备");
    printf("切换后系统输出值: %d\n\n", get_system_output(&main_module, &backup_module));
    
    printf("3. 模拟原主模块恢复:\n");
    simulate_fault(&main_module, 1);
    printf("原主模块恢复后 - ");
    print_module_status(&main_module, "主");
    print_module_status(&backup_module, "备");
    printf("系统输出值: %d\n\n", get_system_output(&main_module, &backup_module));
    
    printf("4. 模拟新主模块(原备用模块)故障:\n");
    simulate_fault(&backup_module, 0);
    printf("新主模块故障后 - ");
    print_module_status(&main_module, "主");
    print_module_status(&backup_module, "备");
    monitor_and_switch(&main_module, &backup_module);
    printf("切换尝试后 - ");
    print_module_status(&main_module, "主");
    print_module_status(&backup_module, "备");
    printf("系统输出值: %d\n\n", get_system_output(&main_module, &backup_module));
    
    printf("=== 测试完成 ===\n");
    
    return 0;
}