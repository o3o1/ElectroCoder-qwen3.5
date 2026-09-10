#!/usr/bin/env python3
# 模拟油田抽油机变频调速节能控制算法的输出

def calculate_frequency(load_ratio):
    """根据负载率计算输出频率"""
    if load_ratio < 0.3:
        return 30.0
    elif load_ratio <= 0.7:
        return 30.0 + (load_ratio - 0.3) * 25.0
    else:
        return 55.0

def adjust_frequency(target_freq):
    """模拟变频器控制过程"""
    print("调节PWM占空比...")
    
    PWM_MAX_VALUE = 3000
    MAX_FREQ = 55.0
    freq_ratio = target_freq / MAX_FREQ
    target_pwm = int(freq_ratio * PWM_MAX_VALUE)
    
    # 模拟硬件寄存器
    pwm_compare_reg = 0
    
    # 模拟初始值
    current_pwm = 1000 if pwm_compare_reg == 0 else pwm_compare_reg
    
    # 模拟调节过程
    while current_pwm != target_pwm:
        if current_pwm < target_pwm:
            current_pwm += 100
            if current_pwm > target_pwm:
                current_pwm = target_pwm
        else:
            current_pwm -= 100
            if current_pwm < target_pwm:
                current_pwm = target_pwm
        
        # 模拟写入硬件寄存器
        pwm_compare_reg = current_pwm
        print(f"当前值: {current_pwm}")
    
    print(f"频率稳定在{target_freq:.2f} Hz")

def main():
    """主函数 - 测试不同负载情况"""
    test_loads = [0.15, 0.45, 0.85]
    
    print("油田抽油机变频调速节能控制算法模拟")
    print("=" * 50)
    
    for i, load in enumerate(test_loads):
        freq = calculate_frequency(load)
        
        print(f"\n测试 {i+1}:")
        print(f"负载率: {load:.2f} -> 目标频率: {freq:.2f} Hz")
        adjust_frequency(freq)
    
    print("\n" + "=" * 50)
    print("模拟完成!")
    
    # 显示算法说明
    print("\n算法说明:")
    print("1. 负载率 < 0.3: 节能模式，固定30Hz")
    print("2. 负载率 0.3~0.7: 线性调节，30Hz~55Hz")
    print("3. 负载率 > 0.7: 额定模式，固定55Hz")
    
    # 验证计算
    print("\n验证计算:")
    test_cases = [
        (0.15, 30.0),   # 轻载
        (0.30, 30.0),   # 边界
        (0.45, 33.75),  # 中载 (30 + (0.45-0.3)*25 = 30 + 3.75 = 33.75)
        (0.70, 55.0),   # 边界
        (0.85, 55.0),   # 重载
    ]
    
    for load, expected in test_cases:
        actual = calculate_frequency(load)
        status = "✓" if abs(actual - expected) < 0.01 else "✗"
        print(f"负载率 {load:.2f}: 计算值={actual:.2f}Hz, 期望值={expected:.2f}Hz {status}")

if __name__ == "__main__":
    main()