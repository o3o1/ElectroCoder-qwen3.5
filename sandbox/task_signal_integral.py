#!/usr/bin/env python3
import math

def trapezoidal_integral(x, y):
    if len(x) < 2 or len(y) < 2 or len(x) != len(y):
        return 0.0
    
    integral = 0.0
    for i in range(1, len(x)):
        dx = x[i] - x[i-1]
        avg_y = (y[i] + y[i-1]) / 2.0
        integral += avg_y * dx
    return integral

def test_constant_function():
    print("=== 测试1: 常数函数 y=2.0 在区间[0,5] ===")
    
    x = [0.0, 1.0, 2.0, 3.0, 4.0, 5.0]
    y = [2.0, 2.0, 2.0, 2.0, 2.0, 2.0]
    
    print(f"时间数组 x: {x}")
    print(f"信号数组 y: {y}")
    
    result = trapezoidal_integral(x, y)
    expected = 10.0
    error = abs(result - expected)
    
    print(f"计算结果: {result:.6f}")
    print(f"理论值: {expected:.6f}")
    print(f"误差: {error:.6f}\n")

def test_linear_function():
    print("=== 测试2: 线性函数 y=x 在区间[0,3] ===")
    
    x = [0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0]
    y = [0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0]
    
    print(f"时间数组 x: {x}")
    print(f"信号数组 y: {y}")
    
    result = trapezoidal_integral(x, y)
    expected = 4.5
    error = abs(result - expected)
    
    print(f"计算结果: {result:.6f}")
    print(f"理论值: {expected:.6f}")
    print(f"误差: {error:.6f}\n")

def test_sine_function():
    print("=== 测试3: 正弦函数 y=sin(x) 在区间[0,π] ===")
    
    n = 11
    x = [i * math.pi / (n - 1) for i in range(n)]
    y = [math.sin(xi) for xi in x]
    
    print(f"时间数组 x: {[f'{xi:.3f}' for xi in x]}")
    print(f"信号数组 y: {[f'{yi:.3f}' for yi in y]}")
    
    result = trapezoidal_integral(x, y)
    expected = 2.0
    error = abs(result - expected)
    
    print(f"计算结果: {result:.6f}")
    print(f"理论值: {expected:.6f}")
    print(f"误差: {error:.6f}\n")

def main():
    print("电力电子信号积分计算 - 梯形积分法")
    print("===================================\n")
    
    test_constant_function()
    test_linear_function()
    test_sine_function()

if __name__ == "__main__":
    main()