import math
import ctypes
import os

# 模拟C语言程序运行
PI = 3.14159265358979323846
BASE_FREQ = 50.0

class HarmonicComponent(ctypes.Structure):
    _fields_ = [
        ("order", ctypes.c_int),
        ("amplitude", ctypes.c_float),
        ("phase", ctypes.c_float)
    ]

def harmonic_analysis(voltage, N, fs):
    f0 = BASE_FREQ
    orders = [1, 3, 5]
    target_freqs = [f0, 3*f0, 5*f0]
    harmonics = []
    
    for h in range(3):
        k = int(target_freqs[h] * N / fs + 0.5)
        if k < 0:
            k = 0
        if k >= N/2:
            k = int(N/2) - 1
        
        real = 0.0
        imag = 0.0
        
        for n in range(N):
            angle = -2.0 * PI * k * n / N
            real += voltage[n] * math.cos(angle)
            imag += voltage[n] * math.sin(angle)
        
        amplitude = math.sqrt(real*real + imag*imag) * 2.0 / N
        phase = math.atan2(imag, real)
        
        harmonic = HarmonicComponent()
        harmonic.order = orders[h]
        harmonic.amplitude = amplitude
        harmonic.phase = phase
        harmonics.append(harmonic)
    
    return harmonics

def generate_compensation(harmonics, N, fs):
    f0 = BASE_FREQ
    compensation = [0.0] * N
    
    for n in range(N):
        comp_val = 0.0
        for h in range(3):
            angle = 2.0 * PI * harmonics[h].order * f0 * n / fs + harmonics[h].phase
            comp_val -= harmonics[h].amplitude * math.sin(angle)
        compensation[n] = comp_val
    
    return compensation

def main():
    N = 200
    fs = 1000.0
    f0 = BASE_FREQ
    
    voltage = []
    for n in range(N):
        t = n / fs
        v = (311.0 * math.sin(2.0 * PI * f0 * t) +
             15.0 * math.sin(2.0 * PI * 3 * f0 * t) +
             8.0 * math.sin(2.0 * PI * 5 * f0 * t))
        voltage.append(v)
    
    harmonics = harmonic_analysis(voltage, N, fs)
    
    print("谐波分析结果：")
    print("次数\t幅值(V)\t相位(rad)")
    for i in range(3):
        print(f"{harmonics[i].order}\t{harmonics[i].amplitude:.3f}\t{harmonics[i].phase:.3f}")
    
    compensation = generate_compensation(harmonics, N, fs)
    
    print("\n前10个补偿信号值：")
    for i in range(10):
        print(f"compensation[{i}] = {compensation[i]:.3f}")

if __name__ == "__main__":
    main()