#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define PI 3.14159265358979323846
#define FS 2000.0f
#define R 0.95f

typedef struct {
    float b0, b1, b2;
    float a1, a2;
} FilterCoeff;

float notch_filter(float input, float *state_x, float *state_y, 
                   float b0, float b1, float b2, float a1, float a2) {
    float output;
    
    output = b0 * input + b1 * state_x[0] + b2 * state_x[1]
             - a1 * state_y[0] - a2 * state_y[1];
    
    state_x[1] = state_x[0];
    state_x[0] = input;
    
    state_y[1] = state_y[0];
    state_y[0] = output;
    
    return output;
}

void calculate_notch_coeff(float freq, FilterCoeff *coeff) {
    float omega0 = 2.0f * PI * freq / FS;
    float cos_omega0 = cosf(omega0);
    
    coeff->b0 = 1.0f;
    coeff->b1 = -2.0f * cos_omega0;
    coeff->b2 = 1.0f;
    
    coeff->a1 = -2.0f * R * cos_omega0;
    coeff->a2 = R * R;
}

float generate_test_signal(int n) {
    float t = n / FS;
    float signal = 0.0f;
    
    signal += 1.0f * sinf(2.0f * PI * 50.0f * t);
    signal += 0.3f * sinf(2.0f * PI * 150.0f * t);
    signal += 0.2f * sinf(2.0f * PI * 250.0f * t);
    signal += 0.15f * sinf(2.0f * PI * 350.0f * t);
    
    signal += 0.05f * ((float)rand() / RAND_MAX - 0.5f);
    
    return signal;
}

int main() {
    srand(time(NULL));
    
    FilterCoeff coeff_3rd, coeff_5th, coeff_7th;
    calculate_notch_coeff(150.0f, &coeff_3rd);
    calculate_notch_coeff(250.0f, &coeff_5th);
    calculate_notch_coeff(350.0f, &coeff_7th);
    
    float state_x1[2] = {0, 0};
    float state_y1[2] = {0, 0};
    float state_x2[2] = {0, 0};
    float state_y2[2] = {0, 0};
    float state_x3[2] = {0, 0};
    float state_y3[2] = {0, 0};
    
    printf("PLL前置抗谐波干扰数字滤波器测试\n");
    printf("采样频率: %.0f Hz\n", FS);
    printf("滤除谐波: 150Hz(3次), 250Hz(5次), 350Hz(7次)\n");
    printf("极点半径: %.2f\n\n", R);
    
    for (int i = 0; i < 20; i++) {
        float input = generate_test_signal(i);
        
        float output1 = notch_filter(input, state_x1, state_y1,
                                    coeff_3rd.b0, coeff_3rd.b1, coeff_3rd.b2,
                                    coeff_3rd.a1, coeff_3rd.a2);
        
        float output2 = notch_filter(output1, state_x2, state_y2,
                                    coeff_5th.b0, coeff_5th.b1, coeff_5th.b2,
                                    coeff_5th.a1, coeff_5th.a2);
        
        float output3 = notch_filter(output2, state_x3, state_y3,
                                    coeff_7th.b0, coeff_7th.b1, coeff_7th.b2,
                                    coeff_7th.a1, coeff_7th.a2);
        
        printf("采样点%d: 输入=%.6f, 输出=%.6f\n", i, input, output3);
    }
    
    return 0;
}