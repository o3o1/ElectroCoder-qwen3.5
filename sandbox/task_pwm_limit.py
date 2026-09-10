def pwm_limit(duty, min_limit, max_limit):
    if duty < min_limit:
        return min_limit
    elif duty > max_limit:
        return max_limit
    else:
        return duty

def main():
    # 测试用例1: 低于下限的情况
    duty1 = 0.02
    min1 = 0.05
    max1 = 0.95
    result1 = pwm_limit(duty1, min1, max1)
    print(f"原始占空比:{duty1:.2f}, 下限:{min1:.2f}, 上限:{max1:.2f} => 限幅后:{result1:.2f}")
    
    # 测试用例2: 正常范围内的情况
    duty2 = 0.6
    min2 = 0.05
    max2 = 0.95
    result2 = pwm_limit(duty2, min2, max2)
    print(f"原始占空比:{duty2:.2f}, 下限:{min2:.2f}, 上限:{max2:.2f} => 限幅后:{result2:.2f}")
    
    # 测试用例3: 高于上限的情况
    duty3 = 0.98
    min3 = 0.05
    max3 = 0.95
    result3 = pwm_limit(duty3, min3, max3)
    print(f"原始占空比:{duty3:.2f}, 下限:{min3:.2f}, 上限:{max3:.2f} => 限幅后:{result3:.2f}")

if __name__ == "__main__":
    main()