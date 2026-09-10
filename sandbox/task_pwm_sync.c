#include <stdio.h>
#include <stdint.h>

#define PERIOD_VALUE 1000

typedef struct {
    uint32_t phase_register;
    uint32_t period_register;
    uint8_t is_master;
} PWM_Module;

void pwm_counter_update(PWM_Module *modules, int num_modules) {
    int master_triggered = 0;
    
    for (int i = 0; i < num_modules; i++) {
        if (modules[i].is_master) {
            uint32_t old_phase = modules[i].phase_register;
            modules[i].phase_register++;
            if (modules[i].phase_register >= modules[i].period_register) {
                modules[i].phase_register = 0;
                if (old_phase == modules[i].period_register - 1) {
                    master_triggered = 1;
                }
            }
        } else {
            modules[i].phase_register++;
            if (modules[i].phase_register >= modules[i].period_register) {
                modules[i].phase_register = 0;
            }
        }
    }
    
    if (master_triggered) {
        for (int i = 0; i < num_modules; i++) {
            if (!modules[i].is_master) {
                modules[i].phase_register = 0;
            }
        }
    }
}

int main() {
    PWM_Module modules[3];
    
    modules[0].phase_register = 0;
    modules[0].period_register = PERIOD_VALUE;
    modules[0].is_master = 1;
    
    modules[1].phase_register = 0;
    modules[1].period_register = PERIOD_VALUE;
    modules[1].is_master = 0;
    
    modules[2].phase_register = 0;
    modules[2].period_register = PERIOD_VALUE;
    modules[2].is_master = 0;
    
    printf("Initial state:\n");
    printf("Step 0: Master=%u, Slave1=%u, Slave2=%u\n", 
           modules[0].phase_register, modules[1].phase_register, modules[2].phase_register);
    
    for (int step = 1; step <= 10; step++) {
        pwm_counter_update(modules, 3);
        printf("Step %d: Master=%u, Slave1=%u, Slave2=%u\n", 
               step, modules[0].phase_register, modules[1].phase_register, modules[2].phase_register);
    }
    
    int steps_to_sync = 0;
    while (modules[0].phase_register != PERIOD_VALUE - 1) {
        pwm_counter_update(modules, 3);
        steps_to_sync++;
    }
    
    pwm_counter_update(modules, 3);
    printf("[SYNC EVENT] Master reached period, all slaves reset!\n");
    printf("Step %d: Master=%u, Slave1=%u, Slave2=%u\n", 
           10 + steps_to_sync + 1, modules[0].phase_register, modules[1].phase_register, modules[2].phase_register);
    
    for (int step = 1; step <= 5; step++) {
        pwm_counter_update(modules, 3);
        printf("Step %d: Master=%u, Slave1=%u, Slave2=%u\n", 
               10 + steps_to_sync + 1 + step, modules[0].phase_register, modules[1].phase_register, modules[2].phase_register);
    }
    
    return 0;
}