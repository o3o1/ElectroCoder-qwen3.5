import subprocess
import os

# 尝试编译C代码
c_code = '''#include <stdio.h>
#include <math.h>

#define SQRT3 1.73205080757f

int svpwm_sector(float Ualpha, float Ubeta) {
    float Vref1 = Ubeta;
    float Vref2 = (SQRT3 * Ualpha - Ubeta) / 2.0f;
    float Vref3 = (-SQRT3 * Ualpha - Ubeta) / 2.0f;
    
    int A = (Vref1 > 0) ? 1 : 0;
    int B = (Vref2 > 0) ? 1 : 0;
    int C = (Vref3 > 0) ? 1 : 0;
    
    int N = 4 * C + 2 * B + A;
    
    switch(N) {
        case 1: return 2;
        case 2: return 6;
        case 3: return 1;
        case 4: return 4;
        case 5: return 3;
        case 6: return 5;
        default: return 0;
    }
}

int main() {
    float test_cases[5][2] = {
        {0.5f, 0.0f},
        {0.2f, 0.4f},
        {-0.3f, 0.3f},
        {0.0f, -0.5f},
        {0.25f, -0.15f}
    };
    
    for(int i = 0; i < 5; i++) {
        float Ualpha = test_cases[i][0];
        float Ubeta = test_cases[i][1];
        int sector = svpwm_sector(Ualpha, Ubeta);
        printf("Test case %d: (%.2f, %.2f) -> Sector %d\\n", 
               i+1, Ualpha, Ubeta, sector);
    }
    
    return 0;
}'''

# 直接使用Python模拟C代码运行
def svpwm_sector(Ualpha, Ubeta):
    SQRT3 = 1.73205080757
    Vref1 = Ubeta
    Vref2 = (SQRT3 * Ualpha - Ubeta) / 2.0
    Vref3 = (-SQRT3 * Ualpha - Ubeta) / 2.0
    
    A = 1 if Vref1 > 0 else 0
    B = 1 if Vref2 > 0 else 0
    C = 1 if Vref3 > 0 else 0
    
    N = 4 * C + 2 * B + A
    
    sector_map = {
        1: 2,
        2: 6,
        3: 1,
        4: 4,
        5: 3,
        6: 5
    }
    
    return sector_map.get(N, 0)

# 测试用例
test_cases = [
    (0.5, 0.0),
    (0.2, 0.4),
    (-0.3, 0.3),
    (0.0, -0.5),
    (0.25, -0.15)
]

print("SVPWM Sector Test Results:")
for i, (Ualpha, Ubeta) in enumerate(test_cases):
    sector = svpwm_sector(Ualpha, Ubeta)
    print(f"Test case {i+1}: ({Ualpha:.2f}, {Ubeta:.2f}) -> Sector {sector}")