#include <stdio.h>

typedef struct {
    float Ua;
    float Ub;
    float Uc;
} ThreePhaseVoltage;

int calculate_sector(ThreePhaseVoltage v) {
    float U1 = v.Uc;
    float U2 = v.Ub;
    float U3 = v.Ua;
    
    int A = (U3 > 0) ? 1 : 0;
    int B = (U2 > 0) ? 1 : 0;
    int C = (U1 > 0) ? 1 : 0;
    
    int N = A + 2*B + 4*C;
    
    switch(N) {
        case 3: return 1;
        case 1: return 2;
        case 5: return 3;
        case 4: return 4;
        case 6: return 5;
        case 2: return 6;
        default: return 0;
    }
}

int main() {
    ThreePhaseVoltage test_cases[4] = {
        {0.5f, -0.25f, -0.25f},
        {-0.2f, 0.4f, -0.2f},
        {0.1f, 0.1f, -0.2f},
        {0.0f, 0.0f, 0.0f}
    };
    
    for (int i = 0; i < 4; i++) {
        int sector = calculate_sector(test_cases[i]);
        printf("电压(%.2f,%.2f,%.2f) → 扇区%d\n", 
               test_cases[i].Ua, test_cases[i].Ub, test_cases[i].Uc, sector);
    }
    
    return 0;
}