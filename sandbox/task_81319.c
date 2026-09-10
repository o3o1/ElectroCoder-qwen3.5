#include <stdio.h>
#include <math.h>

#define TARGET_VOLTAGE 12.0f
#define PWM_ADJUST_RATE 0.02f
#define VOLTAGE_DIFF_UNIT 0.1f
#define PWM_MIN 0.1f
#define PWM_MAX 0.9f

void adjust_pwm_for_emi(float current_voltage, float target_voltage, float *pwm_duty) {
    float voltage_diff = current_voltage - target_voltage;
    float adjust_amount;
    
    if (fabs(voltage_diff) < 0.001f) {
        return;
    }
    
    adjust_amount = (voltage_diff / VOLTAGE_DIFF_UNIT) * PWM_ADJUST_RATE;
    
    *pwm_duty -= adjust_amount;
    
    if (*pwm_duty < PWM_MIN) {
        *pwm_duty = PWM_MIN;
    } else if (*pwm_duty > PWM_MAX) {
        *pwm_duty = PWM_MAX;
    }
}

void test_scenario(const char *name, float initial_pwm, float current_voltage) {
    float pwm_duty = initial_pwm;
    float target_voltage = TARGET_VOLTAGE;
    
    printf("测试场景: %s\n", name);
    printf("  初始占空比: %.3f\n", pwm_duty);
    printf("  当前电压: %.2fV, 目标电压: %.2fV\n", current_voltage, target_voltage);
    
    adjust_pwm_for_emi(current_voltage, target_voltage, &pwm_duty);
    
    printf("  调整后占空比: %.3f\n", pwm_duty);
    printf("  电压差: %.2fV\n\n", current_voltage - target_voltage);
}

int main() {
    printf("车载DC/DC变换器EMI控制算法模拟\n");
    printf("================================\n\n");
    
    test_scenario("a. 电压低于目标 (11.8V)", 0.5f, 11.8f);
    test_scenario("b. 电压高于目标 (12.3V)", 0.5f, 12.3f);
    test_scenario("c. 测试占空比下限保护 (10.0V)", 0.05f, 10.0f);
    test_scenario("d. 测试占空比上限保护 (14.0V)", 0.95f, 14.0f);
    
    return 0;
}