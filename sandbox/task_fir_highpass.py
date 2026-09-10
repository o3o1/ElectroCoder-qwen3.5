import math
import sys

PI = math.pi

def design_fir_highpass(N, fc, fs):
    """设计FIR高通滤波器系数"""
    coeffs = [0.0] * N
    M = (N - 1) // 2
    
    for n in range(N):
        m = n - M
        
        if m == 0:
            h_d = 1.0 - (2.0 * fc / fs)
        else:
            theta = 2.0 * PI * fc * m / fs
            h_d = -math.sin(theta) / (PI * m)
        
        if N == 1:
            w = 1.0
        else:
            n_norm = n / (N - 1)
            w = 0.42 - 0.5 * math.cos(2.0 * PI * n_norm) + 0.08 * math.cos(4.0 * PI * n_norm)
        
        coeffs[n] = h_d * w
    
    return coeffs

def calculate_stopband_attenuation(coeffs, N, fc, fs, stopband_end):
    """计算阻带衰减"""
    num_points = 100
    max_response = 0.0
    
    for i in range(num_points + 1):
        f = stopband_end * i / num_points
        real_sum = 0.0
        imag_sum = 0.0
        
        for k in range(N):
            angle = -2.0 * PI * f * k / fs
            real_sum += coeffs[k] * math.cos(angle)
            imag_sum += coeffs[k] * math.sin(angle)
        
        magnitude = math.sqrt(real_sum * real_sum + imag_sum * imag_sum)
        if magnitude > max_response:
            max_response = magnitude
    
    if max_response > 0:
        return 20.0 * math.log10(max_response)
    else:
        return -float('inf')

def test_case(N, fc, fs, name):
    """测试用例"""
    print(f"\n=== Test Case: {name} ===")
    print(f"Parameters: N={N}, fc={fc:.1f} Hz, fs={fs:.1f} Hz")
    
    coeffs = design_fir_highpass(N, fc, fs)
    
    print("First 5 coefficients:")
    for i in range(min(5, N)):
        print(f"coeff[{i:2d}] = {coeffs[i]:+.6f}")
    
    print("Last 5 coefficients:")
    for i in range(max(N-5, 0), N):
        print(f"coeff[{i:2d}] = {coeffs[i]:+.6f}")
    
    stopband_end = fc - 200.0
    if stopband_end < 0:
        stopband_end = 0
    
    attenuation = calculate_stopband_attenuation(coeffs, N, fc, fs, stopband_end)
    print(f"Stopband attenuation (0-{stopband_end:.1f} Hz): {attenuation:.2f} dB")
    
    return coeffs

def main():
    """主函数"""
    # 主测试用例
    N1 = 31
    fc1 = 1000.0
    fs1 = 8000.0
    
    print("=== Main Test Case ===")
    print(f"Parameters: N={N1}, fc={fc1:.1f} Hz, fs={fs1:.1f} Hz")
    
    coeffs1 = design_fir_highpass(N1, fc1, fs1)
    
    print("First 5 coefficients:")
    for i in range(5):
        print(f"coeff[{i:2d}] = {coeffs1[i]:+.6f}")
    
    print("Last 5 coefficients:")
    for i in range(N1-5, N1):
        print(f"coeff[{i:2d}] = {coeffs1[i]:+.6f}")
    
    stopband_end = fc1 - 200.0
    attenuation = calculate_stopband_attenuation(coeffs1, N1, fc1, fs1, stopband_end)
    print(f"Stopband attenuation (0-{stopband_end:.1f} Hz): {attenuation:.2f} dB")
    
    # 其他测试用例
    test_case(15, 1500.0, 10000.0, "Lower order, higher cutoff")
    test_case(51, 500.0, 16000.0, "Higher order, lower cutoff")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())