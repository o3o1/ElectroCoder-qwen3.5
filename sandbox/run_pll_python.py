#!/usr/bin/env python3
import math
import sys

PI = 3.14159265358979323846
TWO_PI = 2.0 * PI

class PLL:
    def __init__(self):
        self.theta = 0.0           # 内部振荡器相位
        self.f_est = 50.0          # 估计频率 (Hz)
        self.filter_state = 0.0    # 低通滤波器状态
        self.prev_signal = 0.0     # 前一个采样值（用于过零检测）
        self.zero_cross_detected = 0  # 过零检测标志
    
    def low_pass_filter(self, input_val, alpha):
        self.filter_state = alpha * input_val + (1.0 - alpha) * self.filter_state
        return self.filter_state
    
    def detect_zero_crossing(self, current_signal):
        return (self.prev_signal <= 0.0 and current_signal > 0.0)
    
    def update(self, signal, dt, kp, ki):
        # 过零检测
        self.zero_cross_detected = self.detect_zero_crossing(signal)
        self.prev_signal = signal
        
        if self.zero_cross_detected:
            # 计算相位误差：理想过零点相位应为0，实际内部相位为self.theta
            phase_error = -self.theta  # 误差 = 目标相位(0) - 当前相位
            
            # 处理相位卷绕，使误差在[-π, π]范围内
            while phase_error > PI:
                phase_error -= TWO_PI
            while phase_error < -PI:
                phase_error += TWO_PI
            
            # 低通滤波器参数（截止频率约10Hz）
            alpha = 0.05  # 对应时间常数约0.01s
            filtered_error = self.low_pass_filter(phase_error, alpha)
            
            # 更新估计频率：PI控制器
            self.f_est += ki * filtered_error
            
            # 限制频率范围
            if self.f_est < 45.0:
                self.f_est = 45.0
            if self.f_est > 55.0:
                self.f_est = 55.0
        
        # 更新内部振荡器相位
        self.theta += TWO_PI * self.f_est * dt
        
        # 相位模2π运算，保持在[0, 2π)范围内
        if self.theta >= TWO_PI:
            self.theta -= TWO_PI
        if self.theta < 0.0:
            self.theta += TWO_PI

def generate_signal(t, Vm, f, phi):
    return Vm * math.sin(TWO_PI * f * t + phi)

def main():
    # 参数设置
    Vm = 1.0           # 幅值
    f_input = 50.0     # 输入频率 (Hz)
    phi = 0.1          # 初始相位 (弧度)
    fs = 2000.0        # 采样率 (Hz)
    dt = 1.0 / fs      # 采样间隔 (s)
    
    num_samples = 1000    # 总采样点数
    print_interval = 100  # 打印间隔
    
    # 初始化PLL
    pll = PLL()
    
    # PLL控制器参数
    kp = 0.0           # 比例项（当前未使用）
    ki = 0.1           # 积分项
    
    print("单相锁相环(PLL)仿真测试")
    print(f"输入信号: Vm={Vm:.2f}V, f={f_input:.2f}Hz, φ={phi:.2f}rad")
    print(f"采样率: {fs:.0f}Hz, 总采样点数: {num_samples}")
    print()
    
    print(f"{'时间(s)':<10} {'输入信号':<12} {'内部相位':<12} {'估计频率':<12} {'过零检测':<12}")
    print("-" * 60)
    
    steady_state_error_sum = 0.0
    steady_state_count = 0
    
    for i in range(num_samples):
        t = i * dt
        
        # 生成输入信号
        signal = generate_signal(t, Vm, f_input, phi)
        
        # 更新PLL
        pll.update(signal, dt, kp, ki)
        
        # 每100个点打印一次
        if i % print_interval == 0:
            print(f"{t:<10.4f} {signal:<12.6f} {pll.theta:<12.6f} {pll.f_est:<12.6f} {pll.zero_cross_detected:<12}")
        
        # 收集稳态数据（后500个点）
        if i >= 500:
            steady_state_error_sum += abs(pll.f_est - f_input)
            steady_state_count += 1
    
    # 计算稳态频率误差
    steady_state_error = steady_state_error_sum / steady_state_count
    
    print("\n" + "-" * 60)
    print("锁相环收敛结果:")
    print(f"最终估计频率: {pll.f_est:.6f} Hz")
    print(f"目标频率: {f_input:.6f} Hz")
    print(f"稳态频率误差: {steady_state_error:.6f} Hz")
    print(f"相对误差: {(steady_state_error / f_input) * 100.0:.6f}%")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())