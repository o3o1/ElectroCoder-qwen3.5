#include <stdio.h>
#include <math.h>

// 硬件寄存器宏定义
#define PFC_MAX_VOLTAGE 450.0f
#define LLC_MAX_POWER 5000.0f
#define GRID_VOLTAGE_MIN 176.0f
#define GRID_VOLTAGE_MAX 264.0f

// PFC目标母线电压
#define PFC_TARGET_VOLTAGE 400.0f

// LLC充电参数
#define LLC_CONSTANT_CURRENT 10.0f
#define LLC_CHARGE_MARGIN 0.5f

void obc_control(float *pfc_output, float *llc_output, float grid_voltage, float battery_voltage, float battery_soc) {
    // 1. PFC级控制：将电网电压整流升压到稳定的400V DC母线电压
    // 实际应用中会有更复杂的控制算法，这里简化为目标电压控制
    float pfc_target = PFC_TARGET_VOLTAGE;
    
    // 考虑电网电压范围限制
    if (grid_voltage < GRID_VOLTAGE_MIN) {
        // 电网电压过低，降低PFC输出电压
        pfc_target = PFC_TARGET_VOLTAGE * (grid_voltage / GRID_VOLTAGE_MIN);
    } else if (grid_voltage > GRID_VOLTAGE_MAX) {
        // 电网电压过高，限制PFC输出电压
        pfc_target = PFC_TARGET_VOLTAGE * (GRID_VOLTAGE_MAX / grid_voltage);
    }
    
    // 确保不超过最大电压限制
    if (pfc_target > PFC_MAX_VOLTAGE) {
        pfc_target = PFC_MAX_VOLTAGE;
    }
    
    *pfc_output = pfc_target;
    
    // 2. LLC级控制：根据电池SOC动态调整输出
    if (battery_soc < 0.8f) {
        // 恒流充电模式：SOC < 0.8时，输出电流 = 10A
        *llc_output = LLC_CONSTANT_CURRENT;
    } else {
        // 恒压充电模式：SOC ≥ 0.8时，输出电压 = 电池电压 + 0.5V
        // 这里llc_output表示恒压模式下的输出电压
        float target_voltage = battery_voltage + LLC_CHARGE_MARGIN;
        
        // 计算对应的电流（基于功率限制）
        float max_current = LLC_MAX_POWER / target_voltage;
        
        // 在恒压模式下，输出电流由负载决定，这里用负值表示恒压模式
        *llc_output = -target_voltage; // 负值表示恒压模式
    }
}

int main() {
    printf("电动汽车车载充电机（OBC）控制算法测试\n");
    printf("======================================\n\n");
    
    // 测试用例a: 正常充电场景
    {
        float grid_voltage = 220.0f;
        float battery_voltage = 360.0f;
        float battery_soc = 0.6f;
        float pfc_output, llc_output;
        
        obc_control(&pfc_output, &llc_output, grid_voltage, battery_voltage, battery_soc);
        
        printf("测试用例a - 正常充电场景:\n");
        printf("  电网电压: %.1fV\n", grid_voltage);
        printf("  电池电压: %.1fV, SOC: %.1f%%\n", battery_voltage, battery_soc * 100);
        printf("  PFC输出电压: %.1fV\n", pfc_output);
        if (llc_output >= 0) {
            printf("  LLC输出电流: %.1fA (恒流模式)\n", llc_output);
        } else {
            printf("  LLC输出电压: %.1fV (恒压模式)\n", -llc_output);
        }
        printf("\n");
    }
    
    // 测试用例b: 恒压充电场景
    {
        float grid_voltage = 230.0f;
        float battery_voltage = 400.0f;
        float battery_soc = 0.9f;
        float pfc_output, llc_output;
        
        obc_control(&pfc_output, &llc_output, grid_voltage, battery_voltage, battery_soc);
        
        printf("测试用例b - 恒压充电场景:\n");
        printf("  电网电压: %.1fV\n", grid_voltage);
        printf("  电池电压: %.1fV, SOC: %.1f%%\n", battery_voltage, battery_soc * 100);
        printf("  PFC输出电压: %.1fV\n", pfc_output);
        if (llc_output >= 0) {
            printf("  LLC输出电流: %.1fA (恒流模式)\n", llc_output);
        } else {
            printf("  LLC输出电压: %.1fV (恒压模式)\n", -llc_output);
        }
        printf("\n");
    }
    
    // 测试用例c: 边界测试
    {
        float grid_voltage = 180.0f;
        float battery_voltage = 300.0f;
        float battery_soc = 0.95f;
        float pfc_output, llc_output;
        
        obc_control(&pfc_output, &llc_output, grid_voltage, battery_voltage, battery_soc);
        
        printf("测试用例c - 边界测试（低压电网）:\n");
        printf("  电网电压: %.1fV\n", grid_voltage);
        printf("  电池电压: %.1fV, SOC: %.1f%%\n", battery_voltage, battery_soc * 100);
        printf("  PFC输出电压: %.1fV\n", pfc_output);
        if (llc_output >= 0) {
            printf("  LLC输出电流: %.1fA (恒流模式)\n", llc_output);
        } else {
            printf("  LLC输出电压: %.1fV (恒压模式)\n", -llc_output);
        }
        printf("\n");
    }
    
    // 额外测试：电网电压过高场景
    {
        float grid_voltage = 280.0f;
        float battery_voltage = 380.0f;
        float battery_soc = 0.7f;
        float pfc_output, llc_output;
        
        obc_control(&pfc_output, &llc_output, grid_voltage, battery_voltage, battery_soc);
        
        printf("额外测试 - 电网电压过高:\n");
        printf("  电网电压: %.1fV\n", grid_voltage);
        printf("  电池电压: %.1fV, SOC: %.1f%%\n", battery_voltage, battery_soc * 100);
        printf("  PFC输出电压: %.1fV\n", pfc_output);
        if (llc_output >= 0) {
            printf("  LLC输出电流: %.1fA (恒流模式)\n", llc_output);
        } else {
            printf("  LLC输出电压: %.1fV (恒压模式)\n", -llc_output);
        }
    }
    
    return 0;
}