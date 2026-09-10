#include <stdio.h>
#include <stdlib.h>

#define BATTERY_CAPACITY 100.0
#define MAX_POWER 20.0
#define INITIAL_SOC 30.0
#define EFFICIENCY 0.95

typedef enum {
    VALLEY,     // 谷时段 0:00-8:00
    PEAK,       // 峰时段 16:00-20:00
    NORMAL      // 平时段 8:00-16:00, 20:00-24:00
} TimePeriod;

typedef struct {
    double price;
    const char* name;
} PriceInfo;

const PriceInfo price_table[3] = {
    {0.3, "谷时段"},
    {1.2, "峰时段"},
    {0.6, "平时段"}
};

TimePeriod get_time_period(int hour) {
    if (hour >= 0 && hour < 8) {
        return VALLEY;
    } else if (hour >= 8 && hour < 16) {
        return NORMAL;
    } else if (hour >= 16 && hour < 20) {
        return PEAK;
    } else {
        return NORMAL;
    }
}

double calculate_charge(double soc, double power, int hour) {
    TimePeriod period = get_time_period(hour);
    
    if (period == VALLEY) {
        double available_capacity = BATTERY_CAPACITY - soc;
        double max_charge = power * EFFICIENCY;
        
        if (available_capacity >= max_charge) {
            return max_charge;
        } else {
            return available_capacity;
        }
    }
    
    return 0.0;
}

double calculate_discharge(double soc, double power, int hour) {
    TimePeriod period = get_time_period(hour);
    
    if (period == PEAK) {
        double max_discharge = power / EFFICIENCY;
        
        if (soc >= max_discharge) {
            return max_discharge;
        } else {
            return soc;
        }
    }
    
    return 0.0;
}

int main() {
    double soc = INITIAL_SOC;
    double total_charge = 0.0;
    double total_discharge = 0.0;
    double total_cost = 0.0;
    double total_income = 0.0;
    
    printf("储能系统24小时模拟运行\n");
    printf("初始电量: %.2f kWh\n\n", soc);
    
    for (int hour = 0; hour < 24; hour++) {
        TimePeriod period = get_time_period(hour);
        double charge_amount = 0.0;
        double discharge_amount = 0.0;
        double power_used = 0.0;
        double old_soc = soc;
        
        if (period == VALLEY) {
            charge_amount = calculate_charge(soc, MAX_POWER, hour);
            if (charge_amount > 0) {
                power_used = charge_amount / EFFICIENCY;
                soc += charge_amount;
                total_charge += charge_amount;
                total_cost += power_used * price_table[period].price;
            }
        } else if (period == PEAK) {
            discharge_amount = calculate_discharge(soc, MAX_POWER, hour);
            if (discharge_amount > 0) {
                power_used = discharge_amount * EFFICIENCY;
                soc -= discharge_amount;
                total_discharge += discharge_amount;
                total_income += power_used * price_table[period].price;
            }
        }
        
        printf("Hour %2d: %s, ", hour, price_table[period].name);
        
        if (charge_amount > 0) {
            printf("充电%.2fkW, 电量从%.2f增加到%.2f", 
                   charge_amount / EFFICIENCY, old_soc, soc);
        } else if (discharge_amount > 0) {
            printf("放电%.2fkW, 电量从%.2f减少到%.2f", 
                   discharge_amount * EFFICIENCY, old_soc, soc);
        } else {
            printf("保持电量不变, 当前电量%.2f", soc);
        }
        printf("\n");
    }
    
    printf("\n=== 统计结果 ===\n");
    printf("总充电量: %.2f kWh\n", total_charge);
    printf("总放电量: %.2f kWh\n", total_discharge);
    printf("总支出: %.2f 元\n", total_cost);
    printf("总收入: %.2f 元\n", total_income);
    printf("净收益: %.2f 元\n", total_income - total_cost);
    
    return 0;
}