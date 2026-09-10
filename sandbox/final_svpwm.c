#include <stdio.h>
#include <math.h>

#define SQRT3 1.73205080757f

int svpwm_sector(float Ualpha, float Ubeta) {
    // 计算三个参考电压
    float Vref1 = Ubeta;
    float Vref2 = (SQRT3 * Ualpha - Ubeta) / 2.0f;
    float Vref3 = (-SQRT3 * Ualpha - Ubeta) / 2.0f;
    
    // 判断正负
    int A = (Vref1 > 0) ? 1 : 0;
    int B = (Vref2 > 0) ? 1 : 0;
    int C = (Vref3 > 0) ? 1 : 0;
    
    // 根据标准SVPWM扇区判断表
    // N = 4*C + 2*B + A
    int N = 4 * C + 2 * B + A;
    
    // 扇区映射表
    int sector = 0;
    switch(N) {
        case 1: sector = 2; break;  // 001
        case 2: sector = 6; break;  // 010
        case 3: sector = 1; break;  // 011
        case 4: sector = 4; break;  // 100
        case 5: sector = 3; break;  // 101
        case 6: sector = 5; break;  // 110
        default: sector = 0; break; // 000 or 111
    }
    
    return sector;
}

int main() {
    // 测试用例
    float test_cases[5][2] = {
        {0.5f, 0.0f},     // 应在扇区1或2边界
        {0.2f, 0.4f},     // 应在扇区1
        {-0.3f, 0.3f},    // 应在扇区2
        {0.0f, -0.5f},    // 应在扇区5或6边界
        {0.25f, -0.15f}   // 应在扇区6
    };
    
    printf("SVPWM Sector Test Results:\n");
    printf("===========================\n");
    
    for(int i = 0; i < 5; i++) {
        float Ualpha = test_cases[i][0];
        float Ubeta = test_cases[i][1];
        int sector = svpwm_sector(Ualpha, Ubeta);
        
        // 手动计算验证
        float Vref1 = Ubeta;
        float Vref2 = (SQRT3 * Ualpha - Ubeta) / 2.0f;
        float Vref3 = (-SQRT3 * Ualpha - Ubeta) / 2.0f;
        
        printf("Test case %d:\n", i+1);
        printf("  Input: (Ualpha=%.2f, Ubeta=%.2f)\n", Ualpha, Ubeta);
        printf("  Vref1=%.3f, Vref2=%.3f, Vref3=%.3f\n", Vref1, Vref2, Vref3);
        printf("  Sector: %d\n\n", sector);
    }
    
    return 0;
}