import math

def calculate_rms(samples):
    if not samples:
        return 0.0
    
    sum_squares = sum(x * x for x in samples)
    return math.sqrt(sum_squares / len(samples))

# 测试用例 a: 正弦波半个周期
test_a = [0.0, 0.707, 1.0, 0.707, 0.0]
rms_a = calculate_rms(test_a)
print(f"测试用例 a (正弦波半个周期): RMS = {rms_a:.4f}")

# 测试用例 b: 直流序列
test_b = [2.5, 2.5, 2.5, 2.5]
rms_b = calculate_rms(test_b)
print(f"测试用例 b (直流序列): RMS = {rms_b:.4f}")

# 测试用例 c: 包含负值的交流序列
test_c = [-1.0, 0.0, 1.0, 0.0]
rms_c = calculate_rms(test_c)
print(f"测试用例 c (包含负值的交流序列): RMS = {rms_c:.4f}")

# 验证计算
print("\n验证计算:")
print(f"测试用例 a 计算: sqrt((0^2 + 0.707^2 + 1^2 + 0.707^2 + 0^2)/5)")
print(f"= sqrt((0 + 0.499849 + 1 + 0.499849 + 0)/5)")
print(f"= sqrt(1.999698/5) = sqrt(0.3999396) = {rms_a:.4f}")

print(f"\n测试用例 b 计算: sqrt((2.5^2 + 2.5^2 + 2.5^2 + 2.5^2)/4)")
print(f"= sqrt((6.25 + 6.25 + 6.25 + 6.25)/4)")
print(f"= sqrt(25/4) = sqrt(6.25) = {rms_b:.4f}")

print(f"\n测试用例 c 计算: sqrt(((-1)^2 + 0^2 + 1^2 + 0^2)/4)")
print(f"= sqrt((1 + 0 + 1 + 0)/4)")
print(f"= sqrt(2/4) = sqrt(0.5) = {rms_c:.4f}")