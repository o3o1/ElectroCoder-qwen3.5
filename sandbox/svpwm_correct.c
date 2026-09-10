#include <stdio.h>
#include <math.h>

#define SQRT3 1.73205080757f

int svpwm_sector(float Ualpha, float Ubeta) {
    float Vref1 = Ubeta;
    float Vref2 = (SQRT3 * Ualpha - Ubeta) / 2.0f;
    float Vref3 = (-SQRT3 * Ualpha - Ubeta) / 2.0f;
    
    // 正确的扇区判断逻辑
    int sector = 0;
    
    if (Vref1 > 0) {
        if (Vref2 > 0) {
            sector = (Vref3 > 0) ? 0 : 1;  // Vref3<=0 -> 扇区1
        } else {
            sector = (Vref3 > 0) ? 5 : 6;  // Vref3>0 -> 扇区5, Vref3<=0 -> 扇区6
        }
    } else {
        if (Vref2 > 0) {
            sector = (Vref3 > 0) ? 3 : 2;  // Vref3>0 -> 扇区3, Vref3<=0 -> 扇区2
        } else {
            sector = (Vref3 > 0) ? 4 : 0;  // Vref3>0 -> 扇区4
        }
    }
    
    return sector;
}

int main() {
    float test_cases[5][2] = {
        {0.5f, 0.0f},
        {0.2f, 0.4f},
        {-0.3f, 0.3f},
        {0.0f, -0.5f},
        {0.25f, -0.15f}
    };
    
    printf("SVPWM Sector Test Results:\n");
    for(int i = 0; i < 5; i++) {
        float Ualpha = test_cases[i][0];
        float Ubeta = test_cases[i][1];
        int sector = svpwm_sector(Ualpha, Ubeta);
        printf("Test case %d: (%.2f, %.2f) -> Sector %d\n", 
               i+1, Ualpha, Ubeta, sector);
    }
    
    return 0;
}