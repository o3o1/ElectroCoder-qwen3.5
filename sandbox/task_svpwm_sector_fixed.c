#include <stdio.h>
#include <math.h>

#define SQRT3 1.73205080757f

int determine_sector(float Ualpha, float Ubeta) {
    float A = Ubeta;
    float B = 0.5f * (SQRT3 * Ualpha - Ubeta);
    float C = -0.5f * (SQRT3 * Ualpha + Ubeta);
    
    if (A > 0 && C <= 0) return 1;
    if (A <= 0 && B > 0) return 2;
    if (B <= 0 && C > 0) return 3;
    if (A <= 0 && C > 0) return 4;
    if (A > 0 && B <= 0) return 5;
    if (B > 0 && C <= 0) return 6;
    
    return 1;
}

int main() {
    float test_cases[][2] = {
        {1.0f, 0.0f},
        {0.5f, 0.866f},
        {-0.5f, -0.5f},
        {0.0f, 1.0f},
        {-1.0f, 0.0f}
    };
    
    int num_tests = sizeof(test_cases) / sizeof(test_cases[0]);
    
    printf("SVPWM sector test:\n");
    printf("==================\n");
    
    for (int i = 0; i < num_tests; i++) {
        float Ualpha = test_cases[i][0];
        float Ubeta = test_cases[i][1];
        int sector = determine_sector(Ualpha, Ubeta);
        
        printf("Test case %d: Ualpha=%.3f, Ubeta=%.3f -> Sector %d\n", 
               i+1, Ualpha, Ubeta, sector);
    }
    
    return 0;
}