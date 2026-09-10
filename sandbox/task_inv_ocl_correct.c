#include <stdio.h>

#define OVERCURRENT_THRESHOLD 20.0f
#define VOLTAGE 220.0f
#define LOAD_SHEDDING_RATE 0.1f

typedef struct {
    float current;
    float power;
    int fault_flag;
} Inverter;

void read_current(Inverter *inv, float measured_current) {
    inv->current = measured_current;
    if (inv->current > OVERCURRENT_THRESHOLD) {
        inv->fault_flag = 1;
    } else {
        inv->fault_flag = 0;
    }
}

void current_limiting(Inverter *inv) {
    if (inv->current > OVERCURRENT_THRESHOLD) {
        inv->current = OVERCURRENT_THRESHOLD;
    }
}

void load_shedding(Inverter *inv) {
    if (inv->fault_flag) {
        inv->power *= (1.0f - LOAD_SHEDDING_RATE);
    }
}

void update_power(Inverter *inv) {
    inv->power = VOLTAGE * inv->current;
}

int main() {
    Inverter inv;
    float simulated_currents[] = {18.0f, 22.5f, 21.0f, 19.5f, 17.0f};
    int steps = 5;
    
    inv.current = 15.0f;
    update_power(&inv);
    inv.fault_flag = 0;
    
    for (int i = 0; i < steps; i++) {
        read_current(&inv, simulated_currents[i]);
        current_limiting(&inv);
        if (inv.fault_flag) {
            load_shedding(&inv);
        } else {
            update_power(&inv);
        }
        
        printf("Step %d: current=%.2fA, power=%.2fW, fault=%d\n", 
               i+1, inv.current, inv.power, inv.fault_flag);
    }
    
    return 0;
}
