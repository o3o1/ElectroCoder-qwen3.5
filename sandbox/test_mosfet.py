#!/usr/bin/env python3
"""
MOSFET驱动电路模拟测试
模拟死区时间和米勒平台效应
"""

def generate_drive_signals(pwm_duty, dead_time_ns, freq_khz, samples):
    """生成驱动信号"""
    period_us = 1000.0 / freq_khz
    period_ns = period_us * 1000.0
    duty_ratio = pwm_duty / 100.0
    high_on_time_ns = period_ns * duty_ratio
    dead_time_ns_double = dead_time_ns
    
    high_side = [0] * samples
    low_side = [0] * samples
    
    for i in range(samples):
        time_ns = (i * period_ns) / samples
        
        high_on_start = dead_time_ns_double / 2
        high_on_end = high_on_time_ns - dead_time_ns_double / 2
        low_on_start = high_on_time_ns + dead_time_ns_double / 2
        low_on_end = period_ns - dead_time_ns_double / 2
        
        if high_on_start <= time_ns < high_on_end:
            high_side[i] = 1
        
        if low_on_start <= time_ns < low_on_end:
            low_side[i] = 1
    
    return high_side, low_side

def simulate_miller_plateau(gate_voltage, vth, vgp, miller_time_samples):
    """模拟米勒平台效应"""
    plateau_started = False
    plateau_counter = 0
    
    for i in range(len(gate_voltage)):
        if not plateau_started and gate_voltage[i] >= vth:
            plateau_started = True
        
        if plateau_started:
            if plateau_counter < miller_time_samples:
                gate_voltage[i] = vgp
                plateau_counter += 1
            else:
                break
    
    return gate_voltage

def main():
    print("=== MOSFET驱动电路模拟测试 ===\n")
    
    print("测试1：死区时间模拟")
    print("参数：占空比=60%，死区时间=100ns，频率=50kHz，采样点=100")
    
    samples = 100
    high_side, low_side = generate_drive_signals(60, 100, 50, samples)
    
    print("前20个采样点状态：")
    print("采样点\t高边\t低边")
    for i in range(20):
        print(f"{i}\t{high_side[i]}\t{low_side[i]}")
    
    print("\n测试2：米勒平台效应模拟")
    print("参数：vth=3V, vgp=5V, 平台持续时间=20个采样点")
    
    gate_voltage = [(i * 10) // samples for i in range(samples)]
    
    print("原始栅极电压（前30个点）：")
    print(" ".join(str(v) for v in gate_voltage[:30]))
    
    gate_voltage = simulate_miller_plateau(gate_voltage, 3, 5, 20)
    
    print("模拟米勒平台后的栅极电压（前30个点）：")
    print(" ".join(str(v) for v in gate_voltage[:30]))
    
    print("\n详细电压变化过程：")
    print("采样点\t电压(V)")
    for i in range(30):
        print(f"{i}\t{gate_voltage[i]}")
    
    print("\n=== 测试完成 ===")

if __name__ == "__main__":
    main()