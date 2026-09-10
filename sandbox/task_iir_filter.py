#!/usr/bin/env python3
import math

def iir_lowpass(input_val, alpha, prev_output):
    output = alpha * input_val + (1.0 - alpha) * prev_output
    return output, output

def main():
    fs = 1000.0      # 采样频率 1000Hz
    f_signal = 50.0  # 信号频率 50Hz
    f_noise = 200.0  # 噪声频率 200Hz
    alpha = 0.1      # 滤波系数
    
    num_samples = 100  # 5个周期，每个周期20个采样点
    signal = [0.0] * num_samples
    filtered = [0.0] * num_samples
    
    prev_output = 0.0  # 初始状态 y[-1] = 0
    
    # 生成测试信号：正弦波 + 高频噪声
    for i in range(num_samples):
        t = i / fs
        signal[i] = math.sin(2.0 * math.pi * f_signal * t) + 0.3 * math.sin(2.0 * math.pi * f_noise * t)
    
    # 滤波处理
    for i in range(num_samples):
        filtered[i], prev_output = iir_lowpass(signal[i], alpha, prev_output)
    
    # 打印前10个采样点对比
    print("原始信号和滤波后信号对比（前10个采样点）：")
    for i in range(10):
        print(f"原始信号[{i}]={signal[i]:.6f}, 滤波后[{i}]={filtered[i]:.6f}")
    
    # 计算峰值变化
    max_original = 0.0
    max_filtered = 0.0
    
    for i in range(num_samples):
        abs_original = abs(signal[i])
        abs_filtered = abs(filtered[i])
        
        if abs_original > max_original:
            max_original = abs_original
        if abs_filtered > max_filtered:
            max_filtered = abs_filtered
    
    print(f"\n峰值变化: 原始峰值={max_original:.6f}, 滤波后峰值={max_filtered:.6f}")

if __name__ == "__main__":
    main()