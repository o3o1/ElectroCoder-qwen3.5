#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define PI 3.14159265358979323846

typedef struct {
    double pf_target;     // 目标功率因数
    double P_rated;       // 额定有功功率 (W)
    double Q_calculated;  // 计算的无功功率 (var)
    double phi_rad;       // 相位角 (弧度)
} PowerFactorParams;

void calculate_power_factor_params(double pf_target, double P_rated, PowerFactorParams* params) {
    params->pf_target = pf_target;
    params->P_rated = P_rated;
    
    if (fabs(pf_target) > 1.0) {
        printf("警告：功率因数超出范围 [-1.0, 1.0]\n");
        pf_target = pf_target > 0 ? 1.0 : -1.0;
    }
    
    double phi = acos(fabs(pf_target));
    params->phi_rad = (pf_target >= 0) ? phi : -phi;
    
    params->Q_calculated = P_rated * tan(phi);
}

void print_power_factor_table(double P_rated) {
    printf("=== 功率因数参数表 (P = %.0f W) ===\n", P_rated);
    printf("PF\t\tQ (var)\t\tφ (rad)\t\tφ (deg)\n");
    printf("------------------------------------------------\n");
    
    for (double pf = -0.8; pf <= 0.8 + 0.001; pf += 0.2) {
        PowerFactorParams params;
        calculate_power_factor_params(pf, P_rated, &params);
        
        printf("%+.1f\t\t%+8.2f\t%+8.4f\t%+8.2f\n", 
               pf, params.Q_calculated, params.phi_rad, params.phi_rad * 180.0 / PI);
    }
    printf("\n");
}

void simulate_control_loop(double P_rated) {
    printf("=== 控制循环模拟 (P = %.0f W) ===\n", P_rated);
    printf("步长\tPF\t\tQ (var)\t\tφ (rad)\t\t状态\n");
    printf("------------------------------------------------------------\n");
    
    int step = 0;
    for (double pf = -0.8; pf <= 0.8 + 0.001; pf += 0.2) {
        PowerFactorParams params;
        calculate_power_factor_params(pf, P_rated, &params);
        
        const char* state;
        if (pf < 0) {
            state = "超前(感性)";
        } else if (pf > 0) {
            state = "滞后(容性)";
        } else {
            state = "单位功率因数";
        }
        
        printf("%d\t%+.1f\t\t%+8.2f\t%+8.4f\t%s\n", 
               step++, pf, params.Q_calculated, params.phi_rad, state);
    }
    printf("\n");
}

int main() {
    double P_rated = 1000.0;
    
    printf("并网逆变器功率因数可调控制算法模拟\n");
    printf("=====================================\n\n");
    
    print_power_factor_table(P_rated);
    simulate_control_loop(P_rated);
    
    printf("功率因数范围说明：\n");
    printf("  PF > 0: 滞后（容性负载，电流滞后电压）\n");
    printf("  PF < 0: 超前（感性负载，电流超前电压）\n");
    printf("  PF = 0: 纯无功功率\n");
    printf("  PF = ±1: 单位功率因数（纯有功功率）\n");
    
    return 0;
}