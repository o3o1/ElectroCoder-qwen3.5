#include <stdio.h>

#define BATTERY_CAPACITY 100.0f
#define TIME_STEP 1.0f

typedef struct {
    float renewable_power;
    float local_load;
    float battery_soc;
    float grid_power;
} PowerSystem;

void update_power_flow(PowerSystem *ps) {
    float surplus = ps->renewable_power - ps->local_load;
    
    if (surplus > 0) {
        float charge_power = surplus;
        float max_charge = (1.0f - ps->battery_soc) * BATTERY_CAPACITY / TIME_STEP;
        
        if (charge_power > max_charge) {
            charge_power = max_charge;
        }
        
        ps->battery_soc += charge_power * TIME_STEP / BATTERY_CAPACITY;
        ps->grid_power = surplus - charge_power;
    } else {
        float deficit = -surplus;
        float max_discharge = ps->battery_soc * BATTERY_CAPACITY / TIME_STEP;
        
        if (deficit <= max_discharge) {
            ps->battery_soc -= deficit * TIME_STEP / BATTERY_CAPACITY;
            ps->grid_power = 0.0f;
        } else {
            ps->battery_soc = 0.0f;
            ps->grid_power = deficit - max_discharge;
        }
    }
}

int main() {
    printf("新能源消纳控制系统 - 本地负载优先策略\n\n");
    
    PowerSystem test1 = {50.0f, 30.0f, 0.5f, 0.0f};
    printf("--- Test Case 1 ---\n");
    printf("Initial: renewable=%.2f, load=%.2f, soc=%.2f\n", 
           test1.renewable_power, test1.local_load, test1.battery_soc);
    update_power_flow(&test1);
    printf("After: grid_power=%.2f, battery_soc=%.2f\n\n", test1.grid_power, test1.battery_soc);
    
    PowerSystem test2 = {20.0f, 40.0f, 0.3f, 0.0f};
    printf("--- Test Case 2 ---\n");
    printf("Initial: renewable=%.2f, load=%.2f, soc=%.2f\n", 
           test2.renewable_power, test2.local_load, test2.battery_soc);
    update_power_flow(&test2);
    printf("After: grid_power=%.2f, battery_soc=%.2f\n\n", test2.grid_power, test2.battery_soc);
    
    PowerSystem test3 = {80.0f, 30.0f, 0.95f, 0.0f};
    printf("--- Test Case 3 ---\n");
    printf("Initial: renewable=%.2f, load=%.2f, soc=%.2f\n", 
           test3.renewable_power, test3.local_load, test3.battery_soc);
    update_power_flow(&test3);
    printf("After: grid_power=%.2f, battery_soc=%.2f\n\n", test3.grid_power, test3.battery_soc);
    
    return 0;
}