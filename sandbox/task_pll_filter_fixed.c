#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define PI 3.14159265358979323846
#define FS 2000.0
#define R 0.95

typedef struct {
    double b0, b1, b2;
    double a1, a2;
} FilterCoeff;

double notch_filter(double input, double *state_x, double *state_y, 
                   double b0, double b1, double b2, double a1, double a2) {
    double output;
    
    output = b0 * input + b1 * state_x[0] + b2 * state_x[1]
             - a1 * state_y[0] - a2 * state_y[1];
    
    state_x[1] = state_x[0];
    state_x[0] = input;
    
    state_y[1] = state_y[0];
    state_y[0] = output;
    
    return output;
}

void calculate_notch_coeff(double freq, FilterCoeff *coeff) {
    double omega0 = 2.0 * PI * freq / FS;
    double cos_omega0 = cos(omega0);
    
    coeff->b0 = 1.0;
    coeff->b1 = -2.0 * cos_omega0;
    coeff->b2 = 1.0;
    
    coeff->a1 = -2.0 * R * cos_omega0;
    coeff->a2 = R * R;
}

double generate_test_signal(int n) {
    double t = n / FS;
    double signal = 0.0;
    
    signal += 1.0 * sin(2.0 * PI * 50.0 * t);
    signal += 0.3 * sin(2.0 * PI * 150.0 * t);
    signal += 0.2 * sin(2.0 * PI * 250.0 * t);
    signal += 0.15 * sin(2.0 * PI * 350.0 * t);
    
    signal += 0.05 * ((double)rand() / RAND_MAX - 0.5);
    
    return signal;
}

int main() {
    srand(time(NULL));
    
    FilterCoeff coeff_3rd, coeff_5th, coeff_7th;
    calculate_notch_coeff(150.0, &coeff_3rd);
    calculate_notch_coeff(250.0, &coeff_5th);
    calculate_notch_coeff(350.0, &coeff_7th);
    
    double state_x1[2] = {0, 0};
    double state_y1[2] = {0, 0};
    double state_x2[2] = {0, 0};
    double state_y2[2] = {0, 0};
    double state_x3[2] = {0, 0};
    double state_y3[2] = {0, 0};
    
    printf("PLL前置抗谐波干扰数字滤波器测试\n");
    printf("采样频率: %.0f Hz\n", FS);
    printf("滤除谐波: 150Hz(3次), 250Hz(5次), 350Hz(7次)\n");
    printf("极点半径: %.2f\n\n", R);
    
    for (int i = 0; i < 20; i++) {
        double input = generate_test_signal(i);
        
        double output1 = notch_filter(input, state_x1, state_y1,
                                    coeff_3rd.b0, coeff_3rd.b1, coeff_3rd.b2,
                                    coeff_3rd.a1, coeff_3rd.a2);
        
        double output2 = notch_filter(output1, state_x2, state_y2,
                                    coeff_5th.b0, coeff_5th.b1, coeff_5th.b2,
                                    coeff_5th.a1, coeff_5th.a2);
        
        double output3 = notch_filter(output2, state_x3, state_y3,
                                    coeff_7th.b0, coeff_7th.b1, coeff_7th.b2,
                                    coeff_7th.a1, coeff_7th.a2);
        
        printf("采样点%d: 输入=%.6f, 输出=%.6f\n", i, input, output3);
    }
    
    return 0;
}