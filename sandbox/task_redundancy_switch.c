#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define HEALTH_THRESHOLD 60
#define TOTAL_POWER 100

typedef struct {
    int id;
    int status;  // 0=备用, 1=主用, 2=故障
    int health_score;
    int output_power;
} Module;

void init_modules(Module* main_module, Module* backup_module) {
    main_module->id = 1;
    main_module->status = 1;
    main_module->health_score = 85;
    main_module->output_power = TOTAL_POWER;
    
    backup_module->id = 2;
    backup_module->status = 0;
    backup_module->health_score = 90;
    backup_module->output_power = 0;
}

void update_health(Module* module, int cycle) {
    if (module->status == 2) return;
    
    if (cycle == 1) {
        module->health_score = 85;
    } else if (cycle == 2) {
        module->health_score = 75;
    } else if (cycle == 3) {
        module->health_score = 55;
    } else if (cycle == 4) {
        module->health_score = 45;
    } else if (cycle == 5) {
        module->health_score = 30;
    }
    
    if (module->health_score <= 0) {
        module->status = 2;
        module->health_score = 0;
    }
}

int need_switch(Module* main_module, Module* backup_module) {
    if (main_module->status == 2) return 1;
    if (main_module->health_score < HEALTH_THRESHOLD && backup_module->status != 2) return 1;
    return 0;
}

void switch_modules(Module* main_module, Module* backup_module) {
    int temp_id = main_module->id;
    int temp_health = main_module->health_score;
    
    main_module->id = backup_module->id;
    main_module->health_score = backup_module->health_score;
    main_module->status = 1;
    main_module->output_power = TOTAL_POWER;
    
    backup_module->id = temp_id;
    backup_module->health_score = temp_health;
    backup_module->status = 0;
    backup_module->output_power = 0;
}

void print_status(int cycle, Module* main_module, Module* backup_module) {
    printf("周期%d: 主模块[id=%d]功率=%dW, 备用模块[id=%d]功率=%dW, 健康度:主=%d/备=%d\n",
           cycle, main_module->id, main_module->output_power,
           backup_module->id, backup_module->output_power,
           main_module->health_score, backup_module->health_score);
}

int main() {
    Module module1, module2;
    Module *current_main, *current_backup;
    
    init_modules(&module1, &module2);
    current_main = &module1;
    current_backup = &module2;
    
    for (int cycle = 1; cycle <= 5; cycle++) {
        print_status(cycle, current_main, current_backup);
        
        update_health(current_main, cycle);
        update_health(current_backup, cycle);
        
        if (need_switch(current_main, current_backup)) {
            printf("周期%d: [切换事件] ", cycle);
            if (current_main->status == 2) {
                printf("主模块故障，切换到模块%d...\n", current_backup->id);
            } else {
                printf("主模块健康度不足，切换到模块%d...\n", current_backup->id);
            }
            
            switch_modules(current_main, current_backup);
            
            Module* temp = current_main;
            current_main = current_backup;
            current_backup = temp;
            
            printf("       新状态: 主模块[id=%d]功率=%dW, 备用模块[id=%d]功率=%dW\n",
                   current_main->id, current_main->output_power,
                   current_backup->id, current_backup->output_power);
        }
    }
    
    return 0;
}