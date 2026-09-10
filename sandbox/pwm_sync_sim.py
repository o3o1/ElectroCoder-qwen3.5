#!/usr/bin/env python3

PERIOD_VALUE = 1000

class PWM_Module:
    def __init__(self, is_master=False):
        self.phase_register = 0
        self.period_register = PERIOD_VALUE
        self.is_master = is_master

def pwm_counter_update(modules):
    master_triggered = False
    
    for module in modules:
        if module.is_master:
            old_phase = module.phase_register
            module.phase_register += 1
            if module.phase_register >= module.period_register:
                module.phase_register = 0
                if old_phase == module.period_register - 1:
                    master_triggered = True
        else:
            module.phase_register += 1
            if module.phase_register >= module.period_register:
                module.phase_register = 0
    
    if master_triggered:
        for module in modules:
            if not module.is_master:
                module.phase_register = 0
    
    return master_triggered

def main():
    modules = [
        PWM_Module(is_master=True),
        PWM_Module(is_master=False),
        PWM_Module(is_master=False)
    ]
    
    print("Initial state:")
    print(f"Step 0: Master={modules[0].phase_register}, Slave1={modules[1].phase_register}, Slave2={modules[2].phase_register}")
    
    for step in range(1, 11):
        pwm_counter_update(modules)
        print(f"Step {step}: Master={modules[0].phase_register}, Slave1={modules[1].phase_register}, Slave2={modules[2].phase_register}")
    
    steps_to_sync = 0
    while modules[0].phase_register != PERIOD_VALUE - 1:
        pwm_counter_update(modules)
        steps_to_sync += 1
    
    sync_triggered = pwm_counter_update(modules)
    if sync_triggered:
        print("[SYNC EVENT] Master reached period, all slaves reset!")
    print(f"Step {10 + steps_to_sync + 1}: Master={modules[0].phase_register}, Slave1={modules[1].phase_register}, Slave2={modules[2].phase_register}")
    
    for step in range(1, 6):
        pwm_counter_update(modules)
        print(f"Step {10 + steps_to_sync + 1 + step}: Master={modules[0].phase_register}, Slave1={modules[1].phase_register}, Slave2={modules[2].phase_register}")

if __name__ == "__main__":
    main()