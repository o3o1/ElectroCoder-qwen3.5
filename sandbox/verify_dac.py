#!/usr/bin/env python3
# -*- coding: utf-8 -*-

def DAC_SetValue(value):
    """模拟DAC设置函数"""
    DAC_MAX_VALUE = 4095
    DAC_MIN_VALUE = 0
    
    if value > DAC_MAX_VALUE:
        return DAC_MAX_VALUE
    elif value < DAC_MIN_VALUE:
        return DAC_MIN_VALUE
    else:
        return value

def generate_triangle_wave(amplitude, step_size, cycles):
    """模拟三角波生成"""
    DAC_MAX_VALUE = 4095
    DAC_MIN_VALUE = 0
    
    # 限制幅值不超过DAC最大值
    if amplitude > DAC_MAX_VALUE:
        amplitude = DAC_MAX_VALUE
    
    current_value = 0
    change_count = 0
    increasing = True
    
    for cycle in range(cycles):
        print(f"周期 {cycle + 1}:")
        current_value = DAC_MIN_VALUE
        increasing = True
        
        while True:
            # 设置DAC输出值
            dac_output = DAC_SetValue(current_value)
            change_count += 1
            
            # 每10次变化打印一次当前值
            if change_count % 10 == 0:
                print(f"  变化 {change_count}: DAC输出 = {dac_output}")
            
            # 判断方向并更新值
            if increasing:
                # 递增阶段
                if current_value + step_size > amplitude:
                    current_value = amplitude
                else:
                    current_value += step_size
                
                # 检查是否达到幅值
                if current_value >= amplitude:
                    increasing = False
            else:
                # 递减阶段
                if current_value < step_size:
                    current_value = DAC_MIN_VALUE
                else:
                    current_value -= step_size
                
                # 检查是否回到最小值
                if current_value <= DAC_MIN_VALUE:
                    break  # 完成一个周期
        
        print(f"  周期 {cycle + 1} 完成")
    
    print(f"  总变化次数: {change_count}")
    return change_count

def main():
    print("=== STM32 DAC三角波生成模拟 ===\n")
    
    # 测试用例1
    print("测试1: 幅值=2048, 步长=5, 周期=2")
    changes1 = generate_triangle_wave(2048, 5, 2)
    print(f"  预期: 每个周期需要 (2048/5)*2 = 819.2 ≈ 820 次变化 * 2 = 1640 次\n")
    
    # 测试用例2
    print("测试2: 幅值=1000, 步长=2, 周期=1")
    changes2 = generate_triangle_wave(1000, 2, 1)
    print(f"  预期: 每个周期需要 (1000/2)*2 = 1000 次变化\n")
    
    # 测试用例3
    print("测试3: 幅值=5000(应限制为4095), 步长=8, 周期=1")
    changes3 = generate_triangle_wave(5000, 8, 1)
    print(f"  预期: 幅值限制为4095, 每个周期需要 (4095/8)*2 ≈ 1024 次变化\n")
    
    print("=== 所有测试完成 ===")
    
    # 验证结果
    print("\n验证结果:")
    print(f"  测试1实际变化次数: {changes1}")
    print(f"  测试2实际变化次数: {changes2}")
    print(f"  测试3实际变化次数: {changes3}")

if __name__ == "__main__":
    main()