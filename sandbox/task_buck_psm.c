#include <stdio.h>
#include <stdlib.h>

// 模拟硬件寄存器
#define GPIO_PIN_SET 1
#define GPIO_PIN_RESET 0

// PSM控制函数
int psm_control(double v_out, double v_ref, double i_load, double i_threshold) {
    static int last_action = 0; // 0:跳过, 1:发出脉冲
    
    // 重载情况：始终返回1
    if (i_load >= i_threshold) {
        last_action = 1;
        return 1;
    }
    
    // 轻载情况：PSM模式
    double v_lower = v_ref * 0.98;  // 电压下限
    double v_upper = v_ref * 1.02;  // 电压上限
    
    if (v_out < v_lower) {
        // 电压低于下限，必须发出脉冲
        last_action = 1;
        return 1;
    } else if (v_out > v_upper) {
        // 电压高于上限，必须跳过脉冲
        last_action = 0;
        return 0;
    } else {
        // 电压在正常范围内，采用交替策略
        if (last_action == 0) {
            last_action = 1;
            return 1;
        } else {
            last_action = 0;
            return 0;
        }
    }
}

// 测试函数
void test_case_1() {
    printf("测试用例1: 重载情况(i_load > i_threshold)\n");
    printf("参数: v_ref=5.0V, i_threshold=0.5A\n");
    printf("周期\tv_out(V)\ti_load(A)\t动作\n");
    
    double v_ref = 5.0;
    double i_threshold = 0.5;
    
    // 模拟5个周期，负载电流大于阈值
    for (int i = 0; i < 5; i++) {
        double v_out = 4.8 + i * 0.05;  // 电压变化
        double i_load = 1.0;  // 重载
        int action = psm_control(v_out, v_ref, i_load, i_threshold);
        printf("%d\t%.2f\t%.2f\t%s\n", i+1, v_out, i_load, action ? "发出脉冲" : "跳过");
    }
    printf("\n");
}

void test_case_2() {
    printf("测试用例2: 轻载且电压偏低\n");
    printf("参数: v_ref=5.0V, i_threshold=0.5A\n");
    printf("周期\tv_out(V)\ti_load(A)\t动作\n");
    
    double v_ref = 5.0;
    double i_threshold = 0.5;
    
    // 模拟电压低于下限的情况
    for (int i = 0; i < 3; i++) {
        double v_out = 4.85;  // 低于下限(4.9V)
        double i_load = 0.2;  // 轻载
        int action = psm_control(v_out, v_ref, i_load, i_threshold);
        printf("%d\t%.2f\t%.2f\t%s\n", i+1, v_out, i_load, action ? "发出脉冲" : "跳过");
    }
    printf("\n");
}

void test_case_3() {
    printf("测试用例3: 轻载且电压正常，演示PSM交替行为\n");
    printf("参数: v_ref=5.0V, i_threshold=0.5A\n");
    printf("周期\tv_out(V)\ti_load(A)\t动作\n");
    
    double v_ref = 5.0;
    double i_threshold = 0.5;
    
    // 模拟电压在正常范围内，演示交替行为
    for (int i = 0; i < 8; i++) {
        double v_out = 5.01;  // 在正常范围内(4.9V-5.1V)
        double i_load = 0.2;  // 轻载
        int action = psm_control(v_out, v_ref, i_load, i_threshold);
        printf("%d\t%.2f\t%.2f\t%s\n", i+1, v_out, i_load, action ? "发出脉冲" : "跳过");
    }
    printf("\n");
}

int main() {
    printf("Buck变换器PSM控制算法测试\n");
    printf("========================================\n\n");
    
    test_case_1();
    test_case_2();
    test_case_3();
    
    // 额外测试：电压高于上限的情况
    printf("额外测试: 轻载且电压高于上限\n");
    printf("参数: v_ref=5.0V, i_threshold=0.5A\n");
    printf("周期\tv_out(V)\ti_load(A)\t动作\n");
    
    double v_ref = 5.0;
    double i_threshold = 0.5;
    
    for (int i = 0; i < 3; i++) {
        double v_out = 5.15;  // 高于上限(5.1V)
        double i_load = 0.2;  // 轻载
        int action = psm_control(v_out, v_ref, i_load, i_threshold);
        printf("%d\t%.2f\t%.2f\t%s\n", i+1, v_out, i_load, action ? "发出脉冲" : "跳过");
    }
    
    return 0;
}