#include <stdio.h>
#include <math.h>

typedef struct {
    float va[3];
    float vb[3];
    float vc[3];
    float vn;
    float iref_a[3];
    float iref_b[3];
    float iref_c[3];
    float vdc;
    float T_sw;
} InverterState;

void calculate_compensation(InverterState *state, float *i_load, float *i_comp) {
    float i_avg = (i_load[0] + i_load[1] + i_load[2]) / 3.0f;
    
    for (int k = 0; k < 3; k++) {
        float i_zero = i_load[k] - i_avg;
        i_comp[k] = -i_zero * 0.8f;
    }
}

int main() {
    InverterState state = {
        .vdc = 600.0f,
        .T_sw = 0.0001f
    };
    
    float test_cases[3][3] = {
        {10.0f, 10.0f, 10.0f},
        {15.0f, 5.0f, 5.0f},
        {12.0f, 8.0f, 4.0f}
    };
    
    for (int i = 0; i < 3; i++) {
        float i_comp[3] = {0};
        
        printf("测试用例 %d:\n", i + 1);
        printf("负载电流: [%.2f, %.2f, %.2f]\n", 
               test_cases[i][0], test_cases[i][1], test_cases[i][2]);
        
        calculate_compensation(&state, test_cases[i], i_comp);
        
        printf("补偿电流指令: [%.2f, %.2f, %.2f]\n", 
               i_comp[0], i_comp[1], i_comp[2]);
        
        printf("补偿后电流: [%.2f, %.2f, %.2f]\n",
               test_cases[i][0] + i_comp[0],
               test_cases[i][1] + i_comp[1],
               test_cases[i][2] + i_comp[2]);
        printf("\n");
    }
    
    return 0;
}