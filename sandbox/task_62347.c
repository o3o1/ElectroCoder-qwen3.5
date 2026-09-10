#include <stdio.h>
#include <math.h>
#include <stdlib.h>

// 桥臂状态枚举
typedef enum {
    LEADING_ARM_ON,
    LAGGING_ARM_ON,
    BOTH_OFF,
    BOTH_ON
} BridgeState;

// 移相控制器结构体
typedef struct {
    double f_sw;           // 开关频率 (Hz)
    double t_dead;         // 死区时间 (秒)
    double duty_ratio;     // 移相比 (0.0-1.0)
    double phase_shift;    // 移相角 (度)
    BridgeState state;     // 当前桥臂状态
    double t_on_leading;   // 超前桥臂导通时间 (秒)
    double t_on_lagging;   // 滞后桥臂导通时间 (秒)
    double t_off_leading;  // 超前桥臂关断时间 (秒)
    double t_off_lagging;  // 滞后桥臂关断时间 (秒)
} PhaseShiftController;

// 初始化控制器
void init_controller(PhaseShiftController *ctrl, double f_sw, double t_dead, double duty_ratio) {
    ctrl->f_sw = f_sw;
    ctrl->t_dead = t_dead;
    ctrl->duty_ratio = duty_ratio;
    ctrl->phase_shift = 0.0;
    ctrl->state = BOTH_OFF;
    
    // 计算开关周期
    double t_sw = 1.0 / f_sw;
    
    // 计算导通时间
    double t_on = duty_ratio * t_sw;
    
    // 初始化导通时序
    ctrl->t_on_leading = 0.0;
    ctrl->t_on_lagging = 0.0;
    ctrl->t_off_leading = t_on;
    ctrl->t_off_lagging = t_on;
}

// 根据负载电流判断ZVS是否可实现
int calculate_zvs_condition(PhaseShiftController *ctrl, double i_load) {
    // 计算开关周期
    double t_sw = 1.0 / ctrl->f_sw;
    
    // 计算导通时间
    double t_on = ctrl->duty_ratio * t_sw;
    
    // 计算移相角对应的延迟时间
    double t_phase_shift = (ctrl->phase_shift / 180.0) * t_sw;
    
    // 计算滞后桥臂实现ZVS所需的最小电流
    // 假设：需要足够的电流在死区时间内对寄生电容放电
    // 简化模型：i_min = C * V / t_dead，其中C为寄生电容，V为母线电压
    // 这里使用经验公式：i_min = 0.5 * (t_dead / t_sw) * 10.0
    
    double i_min_zvs = 0.5 * (ctrl->t_dead / t_sw) * 10.0;
    
    // 对于滞后桥臂实现ZVS，需要负载电流足够大（正电流）
    // 同时需要考虑移相角的影响：移相角越大，实现ZVS越困难
    double phase_factor = 1.0 - (ctrl->phase_shift / 180.0) * 0.5;
    
    // 调整最小电流要求
    i_min_zvs = i_min_zvs / phase_factor;
    
    // 判断ZVS条件
    if (fabs(i_load) >= i_min_zvs) {
        // 还需要检查电流方向：对于滞后桥臂，通常需要正电流
        // 简化判断：电流绝对值足够大即可
        return 1;
    } else {
        return 0;
    }
}

// 更新移相角并重新计算导通时序
void update_phase_shift(PhaseShiftController *ctrl, double phase_shift) {
    // 限制移相角范围在0-180度
    if (phase_shift < 0.0) phase_shift = 0.0;
    if (phase_shift > 180.0) phase_shift = 180.0;
    
    ctrl->phase_shift = phase_shift;
    
    // 计算开关周期
    double t_sw = 1.0 / ctrl->f_sw;
    
    // 计算导通时间
    double t_on = ctrl->duty_ratio * t_sw;
    
    // 计算移相角对应的延迟时间
    double t_phase_shift = (phase_shift / 180.0) * t_sw;
    
    // 更新导通时序
    // 超前桥臂：在周期开始时导通
    ctrl->t_on_leading = 0.0;
    ctrl->t_off_leading = t_on;
    
    // 滞后桥臂：延迟移相角时间后导通
    ctrl->t_on_lagging = t_phase_shift;
    ctrl->t_off_lagging = t_phase_shift + t_on;
    
    // 如果导通时间超过周期，进行调整
    if (ctrl->t_off_lagging > t_sw) {
        ctrl->t_off_lagging = t_sw;
    }
}

int main() {
    PhaseShiftController ctrl;
    
    // 初始化控制器参数
    init_controller(&ctrl, 100000.0, 200e-9, 0.4);
    
    printf("全桥变换器移相控制ZVS算法测试\n");
    printf("开关频率: %.0f Hz\n", ctrl.f_sw);
    printf("死区时间: %.0f ns\n", ctrl.t_dead * 1e9);
    printf("移相比: %.2f\n", ctrl.duty_ratio);
    printf("\n");
    
    // 测试不同移相角下的ZVS条件
    double phase_shifts[] = {0.0, 45.0, 90.0, 135.0, 180.0};
    int num_phases = sizeof(phase_shifts) / sizeof(phase_shifts[0]);
    
    for (int p = 0; p < num_phases; p++) {
        update_phase_shift(&ctrl, phase_shifts[p]);
        
        printf("移相角: %.1f 度\n", phase_shifts[p]);
        printf("负载电流测试:\n");
        
        // 测试负载电流从-5A到5A变化
        for (double i_load = -5.0; i_load <= 5.0; i_load += 1.0) {
            int zvs_possible = calculate_zvs_condition(&ctrl, i_load);
            
            printf("  负载电流: %5.2f A, ZVS状态: %s\n", 
                   i_load, 
                   zvs_possible ? "可实现" : "不可实现");
        }
        printf("\n");
    }
    
    // 详细测试特定移相角下的临界电流
    printf("临界电流测试 (移相角=90度):\n");
    update_phase_shift(&ctrl, 90.0);
    
    for (double i_load = 0.0; i_load <= 2.0; i_load += 0.1) {
        int zvs_possible = calculate_zvs_condition(&ctrl, i_load);
        
        if (zvs_possible) {
            printf("  临界电流约为: %.2f A\n", i_load);
            break;
        }
    }
    
    return 0;
}