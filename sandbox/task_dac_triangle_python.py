#!/usr/bin/env python3
# -*- coding: utf-8 -*-

def generate_triangle_wave(amplitude, step, cycles):
    """模拟DAC三角波生成"""
    dac_value = 0
    increasing = True
    point_count = 0
    
    for cycle in range(cycles):
        # 递增阶段
        increasing = True
        while increasing:
            # 模拟DAC输出
            print(f"DAC输出: {dac_value}", end='')
            point_count += 1
            
            # 每10个点换行
            if point_count % 10 == 0:
                print()
            else:
                print(" ", end='')
            
            # 递增
            if dac_value + step > amplitude:
                dac_value = amplitude
                increasing = False
            else:
                dac_value += step
            
            # 如果已经达到峰值，切换到递减
            if dac_value >= amplitude:
                increasing = False
        
        # 递减阶段
        while not increasing and dac_value > 0:
            # 模拟DAC输出
            print(f"DAC输出: {dac_value}", end='')
            point_count += 1
            
            # 每10个点换行
            if point_count % 10 == 0:
                print()
            else:
                print(" ", end='')
            
            # 递减
            if dac_value < step:
                dac_value = 0
            else:
                dac_value -= step
        
        # 输出最后一个0点（如果还没输出）
        if dac_value == 0 and cycle < cycles - 1:
            print(f"DAC输出: {dac_value}", end='')
            point_count += 1
            
            if point_count % 10 == 0:
                print()
            else:
                print(" ", end='')
    
    # 确保最后换行
    if point_count % 10 != 0:
        print()

def main():
    """主函数"""
    # 测试用例1
    print("=== 测试参数: amplitude=1000, step=10, cycles=2 ===")
    generate_triangle_wave(1000, 10, 2)
    print()
    
    # 测试用例2
    print("=== 测试参数: amplitude=200, step=5, cycles=3 ===")
    generate_triangle_wave(200, 5, 3)

if __name__ == "__main__":
    main()