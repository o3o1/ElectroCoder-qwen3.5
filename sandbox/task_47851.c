#include <stdio.h>
#include <math.h>

#define PWM_ON 1
#define PWM_OFF 0

double calculate_compensated_current(double inductor_current, double compensation_slope, double switching_period, double duty_cycle) {
    double compensation = compensation_slope * duty_cycle * switching_period;
    return inductor_current - compensation;
}

int pwm_compare(double sampled_current, double compensated_ref, double ramp_amplitude) {
    if (sampled_current > compensated_ref) {
        return PWM_OFF;
    } else {
        return PWM_ON;
    }
}

void simulate_subharmonic_scenario() {
    printf("\n=== 次谐波抑制场景模拟 ===\n");
    
    double compensation_slope = 1000.0;
    double switching_period = 10e-6;
    double duty_cycle = 0.6;
    double ramp_amplitude = 1.0;
    
    double inductor_current_ref = 2.5;
    double compensated_ref = calculate_compensated_current(inductor_current_ref, compensation_slope, switching_period, duty_cycle);
    
    printf("原始电流参考值: %.4f A\n", inductor_current_ref);
    printf("补偿后电流参考值: %.4f A\n", compensated_ref);
    
    double sampled_currents[] = {2.3, 2.45};
    
    for (int cycle = 0; cycle < 2; cycle++) {
        printf("\n周期 %d:\n", cycle + 1);
        printf("采样电流: %.4f A\n", sampled_currents[cycle]);
        
        int pwm_state = pwm_compare(sampled_currents[cycle], compensated_ref, ramp_amplitude);
        printf("PWM开关状态: %s\n", pwm_state == PWM_ON ? "开启" : "关闭");
        
        double error = sampled_currents[cycle] - compensated_ref;
        printf("误差: %.4f A\n", error);
        
        if (cycle == 1) {
            double delta_error = sampled_currents[1] - sampled_currents[0];
            printf("电流变化量: %.4f A\n", delta_error);
            
            if (fabs(delta_error) < 0.1) {
                printf("分析: 斜率补偿有效抑制了次谐波振荡\n");
            } else {
                printf("分析: 可能存在次谐波振荡风险\n");
            }
        }
    }
}

int main() {
    printf("=== 正激变换器峰值电流模式控制斜率补偿算法模拟 ===\n\n");
    
    printf("测试用例 a):\n");
    double inductor_current = 2.5;
    double compensation_slope = 1000.0;
    double switching_period = 10e-6;
    double duty_cycle = 0.6;
    
    double compensated_current = calculate_compensated_current(inductor_current, compensation_slope, switching_period, duty_cycle);
    printf("电感电流: %.4f A\n", inductor_current);
    printf("补偿斜率: %.0f A/s\n", compensation_slope);
    printf("开关周期: %.8f s (10μs)\n", switching_period);
    printf("占空比: %.2f\n", duty_cycle);
    printf("补偿后电流参考值: %.6f A\n", compensated_current);
    printf("补偿量: %.6f A\n", compensation_slope * duty_cycle * switching_period);
    
    printf("\n测试用例 b):\n");
    double sampled_current = 2.3;
    double compensated_ref = 2.4;
    double ramp_amplitude = 1.0;
    
    int pwm_state = pwm_compare(sampled_current, compensated_ref, ramp_amplitude);
    printf("采样电流: %.4f A\n", sampled_current);
    printf("补偿后参考值: %.4f A\n", compensated_ref);
    printf("三角波幅值: %.1f V\n", ramp_amplitude);
    printf("PWM开关状态: %s\n", pwm_state == PWM_ON ? "开启" : "关闭");
    
    simulate_subharmonic_scenario();
    
    printf("\n=== 总结 ===\n");
    printf("1. 斜率补偿通过降低电流参考值来稳定系统\n");
    printf("2. 当占空比>50%时，补偿斜率通常取电感电流下降斜率的一半\n");
    printf("3. 补偿公式: I_ref_comp = I_ref - Se * D * Ts\n");
    printf("4. PWM比较器根据采样电流与补偿参考值的比较决定开关状态\n");
    
    return 0;
}