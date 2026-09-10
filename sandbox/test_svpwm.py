import math

def svpwm_sector_detection(Ualpha, Ubeta):
    EPS = 1e-6
    SQRT3_2 = math.sqrt(3)/2
    
    V1 = Ubeta
    V2 = SQRT3_2 * Ualpha - 0.5 * Ubeta
    V3 = -SQRT3_2 * Ualpha - 0.5 * Ubeta
    
    A = 1 if V1 > EPS else 0
    B = 1 if V2 > EPS else 0
    C = 1 if V3 > EPS else 0
    
    N = A + 2 * B + 4 * C
    
    sector_map = {3:1, 1:2, 5:3, 4:4, 6:5, 2:6}
    return sector_map.get(N, 0)

test_cases = [
    (1.0, 0.0),
    (0.5, math.sqrt(3)/2),
    (-0.5, math.sqrt(3)/2),
    (-1.0, 0.0),
    (-0.5, -math.sqrt(3)/2),
    (0.5, -math.sqrt(3)/2)
]

expected = [1, 2, 3, 4, 5, 6]

print("SVPWM扇区判断算法测试结果：")
for i, (Ualpha, Ubeta) in enumerate(test_cases):
    sector = svpwm_sector_detection(Ualpha, Ubeta)
    status = "✓" if sector == expected[i] else f"✗ (期望: {expected[i]})"
    print(f"测试用例 {i+1}: ({Ualpha:.3f}, {Ubeta:.3f}) -> 扇区{sector} {status}")