#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define MAX_SAMPLES 5

double calculate_reactive_current(double grid_voltage, double load_current, 
                                  double power_factor_target, double sampling_time) {
    if (fabs(grid_voltage) < 1e-6) {
        return 0.0;
    }
    
    if (power_factor_target < 0.9 || power_factor_target > 1.0) {
        return 0.0;
    }
    
    double p_load = grid_voltage * load_current;
    double p_target = p_load * power_factor_target;
    double q_comp = p_load * sqrt(1.0 - power_factor_target * power_factor_target);
    double i_comp = q_comp / grid_voltage;
    
    return i_comp;
}

int main() {
    double grid_voltages[MAX_SAMPLES] = {220.0, 218.0, 222.0, 219.0, 221.0};
    double load_currents[MAX_SAMPLES] = {10.0, 9.5, 10.2, 9.8, 10.1};
    double power_factors[MAX_SAMPLES] = {0.95, 0.95, 0.98, 0.96, 0.97};
    double sampling_time = 0.001;
    
    double total_comp_current = 0.0;
    
    printf("SVG无功补偿控制算法模拟\n");
    printf("========================================\n");
    
    for (int i = 0; i < MAX_SAMPLES; i++) {
        double comp_current = calculate_reactive_current(
            grid_voltages[i], 
            load_currents[i], 
            power_factors[i], 
            sampling_time
        );
        
        printf("采样点%d:\n", i + 1);
        printf("  电网电压: %.2f V\n", grid_voltages[i]);
        printf("  负载电流: %.2f A\n", load_currents[i]);
        printf("  目标功率因数: %.2f\n", power_factors[i]);
        printf("  补偿电流: %.4f A\n", comp_current);
        printf("----------------------------------------\n");
        
        total_comp_current += comp_current;
    }
    
    double avg_comp_current = total_comp_current / MAX_SAMPLES;
    printf("平均补偿电流: %.4f A\n", avg_comp_current);
    
    return 0;
}