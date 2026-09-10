#include <stdio.h>
#include <math.h>
#include <stdlib.h>

// Mock hardware registers
#define ADC_VALUE_REG (*((volatile uint16_t*)0x40012440))
#define TIMER_COUNTER (*((volatile uint32_t*)0x40001000))
#define SYS_CLK_FREQ 72000000U

// PLL结构体定义
typedef struct {
    double phase;           // 当前相位 (rad)
    double frequency;       // 当前频率 (Hz)
    double integrator;      // 环路滤波器积分项
    double kp;              // 比例增益
    double ki;              // 积分增益
    double center_freq;     // 中心频率 (Hz)
    double ts;              // 采样周期 (s)
} PLL_TypeDef;

// PLL初始化函数
void PLL_Init(PLL_TypeDef *pll, double center_freq, double bandwidth, double ts) {
    pll->phase = 0.0;
    pll->frequency = center_freq;
    pll->integrator = 0.0;
    pll->center_freq = center_freq;
    pll->ts = ts;
    
    // 根据带宽设置PI参数（简化设计）
    double wn = 2.0 * M_PI * bandwidth;  // 自然频率
    pll->kp = 2.0 * wn;
    pll->ki = wn * wn;
}

// PLL更新函数（每个采样周期调用一次）
void PLL_Update(PLL_TypeDef *pll, double input_voltage) {
    // 1. 生成本地振荡器信号
    double local_signal = sin(pll->phase);
    
    // 2. 相位检测器（乘法器）
    double phase_error = input_voltage * local_signal;
    
    // 3. 环路滤波器（PI控制器）
    double proportional = pll->kp * phase_error;
    pll->integrator += pll->ki * phase_error * pll->ts;
    double control_signal = proportional + pll->integrator;
    
    // 4. 压控振荡器（VCO）
    // 频率 = 中心频率 + 控制信号
    pll->frequency = pll->center_freq + control_signal;
    
    // 5. 积分频率得到相位（模2π）
    pll->phase += 2.0 * M_PI * pll->frequency * pll->ts;
    if (pll->phase > 2.0 * M_PI) {
        pll->phase -= 2.0 * M_PI;
    }
}

// 模拟电网电压信号
double simulate_grid_voltage(double time, double amplitude, double freq, double phase_offset) {
    return amplitude * sin(2.0 * M_PI * freq * time + phase_offset);
}

int main() {
    // 仿真参数
    const double fs = 10000.0;          // 采样频率 10kHz
    const double ts = 1.0 / fs;         // 采样周期
    const double sim_time = 0.1;        // 仿真时间 0.1秒
    const int num_samples = (int)(sim_time * fs);  // 总采样点数
    
    // 电网参数
    const double amplitude = 1.0;       // 电压幅值
    double grid_freq = 50.0;            // 电网频率 (Hz)
    const double phase_offset = 0.2;    // 初始相位偏移 (rad)
    
    // PLL参数
    const double pll_center_freq = 50.0;  // PLL中心频率
    const double pll_bandwidth = 5.0;     // PLL带宽 (Hz)
    
    // 初始化PLL
    PLL_TypeDef pll;
    PLL_Init(&pll, pll_center_freq, pll_bandwidth, ts);
    
    // 统计变量
    double freq_error_sum = 0.0;
    double max_freq_error = 0.0;
    double phase_error_sum = 0.0;
    double max_phase_error = 0.0;
    int error_count = 0;
    
    printf("时间(s)\t输入电压(V)\t估计频率(Hz)\t相位误差(rad)\n");
    printf("--------------------------------------------------------\n");
    
    // 主仿真循环
    for (int i = 0; i < num_samples; i++) {
        double time = i * ts;
        
        // 模拟电网频率缓慢变化（49.5-50.5Hz）
        if (i > num_samples / 2) {
            grid_freq = 50.5;  // 后半段频率变化到50.5Hz
        }
        
        // 生成电网电压信号
        double grid_voltage = simulate_grid_voltage(time, amplitude, grid_freq, phase_offset);
        
        // 模拟ADC采样（mock硬件寄存器）
        uint16_t adc_value = (uint16_t)((grid_voltage + 1.0) * 2048.0);  // 假设12位ADC，0-3.3V范围
        ADC_VALUE_REG = adc_value;
        
        // 模拟定时器（mock硬件寄存器）
        TIMER_COUNTER = (uint32_t)(time * SYS_CLK_FREQ);
        
        // 更新PLL
        PLL_Update(&pll, grid_voltage);
        
        // 计算相位误差（实际相位与估计相位的差值）
        double actual_phase = fmod(2.0 * M_PI * grid_freq * time + phase_offset, 2.0 * M_PI);
        double phase_error = actual_phase - pll.phase;
        // 将相位误差归一化到[-π, π]范围
        if (phase_error > M_PI) phase_error -= 2.0 * M_PI;
        if (phase_error < -M_PI) phase_error += 2.0 * M_PI;
        
        // 计算频率误差
        double freq_error = grid_freq - pll.frequency;
        
        // 更新统计
        freq_error_sum += fabs(freq_error);
        phase_error_sum += fabs(phase_error);
        if (fabs(freq_error) > max_freq_error) {
            max_freq_error = fabs(freq_error);
        }
        if (fabs(phase_error) > max_phase_error) {
            max_phase_error = fabs(phase_error);
        }
        error_count++;
        
        // 每100个点打印一次结果
        if (i % 100 == 0) {
            printf("%.4f\t%.4f\t\t%.4f\t\t%.4f\n", 
                   time, grid_voltage, pll.frequency, phase_error);
        }
    }
    
    // 打印统计结果
    printf("\n=== PLL跟踪性能统计 ===\n");
    printf("仿真点数: %d\n", num_samples);
    printf("平均频率误差: %.6f Hz\n", freq_error_sum / error_count);
    printf("最大频率误差: %.6f Hz\n", max_freq_error);
    printf("平均相位误差: %.6f rad\n", phase_error_sum / error_count);
    printf("最大相位误差: %.6f rad\n", max_phase_error);
    
    return 0;
}