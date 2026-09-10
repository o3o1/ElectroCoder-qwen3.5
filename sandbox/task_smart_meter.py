import math
import os

SAMPLE_COUNT = 10
SAMPLE_FREQ = 1000
SAMPLE_INTERVAL = 0.001
VOLTAGE_PEAK = 311.0
CURRENT = 5.0
PULSE_THRESHOLD = 0.1
PI = 3.14159265358979323846

def calculate_rms(samples):
    sum_sq = 0.0
    for sample in samples:
        sum_sq += sample * sample
    return math.sqrt(sum_sq / len(samples))

def generate_balanced_samples():
    phase_a = []
    phase_b = []
    phase_c = []
    
    for i in range(SAMPLE_COUNT):
        angle = 2 * PI * i / SAMPLE_COUNT
        phase_a.append(VOLTAGE_PEAK * math.sin(angle))
        phase_b.append(VOLTAGE_PEAK * math.sin(angle - 2 * PI / 3))
        phase_c.append(VOLTAGE_PEAK * math.sin(angle + 2 * PI / 3))
    
    return phase_a, phase_b, phase_c

def generate_unbalanced_samples():
    phase_a = []
    phase_b = []
    phase_c = []
    
    for i in range(SAMPLE_COUNT):
        angle = 2 * PI * i / SAMPLE_COUNT
        phase_a.append(1.136 * VOLTAGE_PEAK * math.sin(angle))
        phase_b.append(VOLTAGE_PEAK * math.sin(angle - 2 * PI / 3))
        phase_c.append(VOLTAGE_PEAK * math.sin(angle + 2 * PI / 3))
    
    return phase_a, phase_b, phase_c

def simulate_energy_accumulation(ua_rms, ub_rms, uc_rms, test_case):
    total_energy = 0.0
    energy_since_last_pulse = 0.0
    pulse_count = 0
    
    print(f"测试用例{test_case} - 能量累计模拟:")
    
    while total_energy < 0.5:
        power = (ua_rms + ub_rms + uc_rms) * CURRENT
        energy_increment = power * SAMPLE_INTERVAL / 3600000.0
        
        total_energy += energy_increment
        energy_since_last_pulse += energy_increment
        
        if energy_since_last_pulse >= PULSE_THRESHOLD:
            pulse_count += 1
            print(f"  脉冲事件{pulse_count}: 累计电量 {total_energy:.3f} kWh")
            energy_since_last_pulse = 0.0
    
    print(f"  总累计电量: {total_energy:.3f} kWh, 总脉冲数: {pulse_count}\n")

def main():
    print("智能电表模拟程序")
    print("================")
    print()
    
    print("测试用例1: 三相平衡情况")
    phase_a1, phase_b1, phase_c1 = generate_balanced_samples()
    
    ua_rms1 = calculate_rms(phase_a1)
    ub_rms1 = calculate_rms(phase_b1)
    uc_rms1 = calculate_rms(phase_c1)
    
    print(f"  A相电压有效值: {ua_rms1:.2f} V")
    print(f"  B相电压有效值: {ub_rms1:.2f} V")
    print(f"  C相电压有效值: {uc_rms1:.2f} V")
    
    simulate_energy_accumulation(ua_rms1, ub_rms1, uc_rms1, 1)
    
    print("测试用例2: A相电压偏高情况")
    phase_a2, phase_b2, phase_c2 = generate_unbalanced_samples()
    
    ua_rms2 = calculate_rms(phase_a2)
    ub_rms2 = calculate_rms(phase_b2)
    uc_rms2 = calculate_rms(phase_c2)
    
    print(f"  A相电压有效值: {ua_rms2:.2f} V")
    print(f"  B相电压有效值: {ub_rms2:.2f} V")
    print(f"  C相电压有效值: {uc_rms2:.2f} V")
    
    simulate_energy_accumulation(ua_rms2, ub_rms2, uc_rms2, 2)

if __name__ == "__main__":
    main()