#include <stdio.h>
#include <math.h>
#include <stdint.h>

#define PI 3.14159265358979323846
#define TWO_PI (2.0 * PI)

// PLL结构体
typedef struct {
    double theta;           // 内部振荡器相位
    double f_est;           // 估计频率 (Hz)
    double filter_state;    // 低通滤波器状态
    double prev_signal;     // 前一个采样值（用于过零检测）
    int zero_cross_detected; // 过零检测标志
} PLL;

// 数字低通滤波器（一阶RC滤波器）
double low_pass_filter(double input, double* state, double alpha) {
    *state = alpha * input + (1.0 - alpha) * (*state);
    return *state;
}

// 过零检测函数：检测从负到正的过零点
int detect_zero_crossing(double current_signal, double prev_signal) {
    return (prev_signal <= 0.0 && current_signal > 0.0);
}

// PLL更新函数
void pll_update(PLL* pll, double signal, double dt, double kp, double ki) {
    // 过零检测
    pll->zero_cross_detected = detect_zero_crossing(signal, pll->prev_signal);
    pll->prev_signal = signal;
    
    if (pll->zero_cross_detected) {
        // 计算相位误差：理想过零点相位应为0，实际内部相位为pll->theta
        double phase_error = -pll->theta;  // 误差 = 目标相位(0) - 当前相位
        
        // 处理相位卷绕，使误差在[-π, π]范围内
        while (phase_error > PI) phase_error -= TWO_PI;
        while (phase_error < -PI) phase_error += TWO_PI;
        
        // 低通滤波器参数（截止频率约10Hz）
        double alpha = 0.05;  // 对应时间常数约0.01s
        double filtered_error = low_pass_filter(phase_error, &pll->filter_state, alpha);
        
        // 更新估计频率：PI控制器
        pll->f_est += ki * filtered_error;
        
        // 限制频率范围
        if (pll->f_est < 45.0) pll->f_est = 45.0;
        if (pll->f_est > 55.0) pll->f_est = 55.0;
    }
    
    // 更新内部振荡器相位
    pll->theta += TWO_PI * pll->f_est * dt;
    
    // 相位模2π运算，保持在[0, 2π)范围内
    if (pll->theta >= TWO_PI) pll->theta -= TWO_PI;
    if (pll->theta < 0.0) pll->theta += TWO_PI;
}

// 模拟单相交流信号
double generate_signal(double t, double Vm, double f, double phi) {
    return Vm * sin(TWO_PI * f * t + phi);
}

int main() {
    // 参数设置
    double Vm = 1.0;           // 幅值
    double f_input = 50.0;     // 输入频率 (Hz)
    double phi = 0.1;          // 初始相位 (弧度)
    double fs = 2000.0;        // 采样率 (Hz)
    double dt = 1.0 / fs;      // 采样间隔 (s)
    
    int num_samples = 1000;    // 总采样点数
    int print_interval = 100;  // 打印间隔
    
    // 初始化PLL
    PLL pll;
    pll.theta = 0.0;           // 初始相位
    pll.f_est = 50.0;          // 初始频率估计
    pll.filter_state = 0.0;    // 滤波器初始状态
    pll.prev_signal = 0.0;     // 前一个信号值
    pll.zero_cross_detected = 0;
    
    // PLL控制器参数
    double kp = 0.0;           // 比例项（当前未使用）
    double ki = 0.1;           // 积分项
    
    printf("单相锁相环(PLL)仿真测试\n");
    printf("输入信号: Vm=%.2fV, f=%.2fHz, φ=%.2frad\n", Vm, f_input, phi);
    printf("采样率: %.0fHz, 总采样点数: %d\n\n", fs, num_samples);
    
    printf("%-10s %-12s %-12s %-12s %-12s\n", 
           "时间(s)", "输入信号", "内部相位", "估计频率", "过零检测");
    printf("------------------------------------------------------------\n");
    
    double steady_state_error_sum = 0.0;
    int steady_state_count = 0;
    
    for (int i = 0; i < num_samples; i++) {
        double t = i * dt;
        
        // 生成输入信号
        double signal = generate_signal(t, Vm, f_input, phi);
        
        // 更新PLL
        pll_update(&pll, signal, dt, kp, ki);
        
        // 每100个点打印一次
        if (i % print_interval == 0) {
            printf("%-10.4f %-12.6f %-12.6f %-12.6f %-12d\n", 
                   t, signal, pll.theta, pll.f_est, pll.zero_cross_detected);
        }
        
        // 收集稳态数据（后500个点）
        if (i >= 500) {
            steady_state_error_sum += fabs(pll.f_est - f_input);
            steady_state_count++;
        }
    }
    
    // 计算稳态频率误差
    double steady_state_error = steady_state_error_sum / steady_state_count;
    
    printf("\n------------------------------------------------------------\n");
    printf("锁相环收敛结果:\n");
    printf("最终估计频率: %.6f Hz\n", pll.f_est);
    printf("目标频率: %.6f Hz\n", f_input);
    printf("稳态频率误差: %.6f Hz\n", steady_state_error);
    printf("相对误差: %.6f%%\n", (steady_state_error / f_input) * 100.0);
    
    return 0;
}