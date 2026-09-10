#include <stdio.h>
#include <math.h>

double calculate_true_rms(const double samples[], int n) {
    double sum_squares = 0.0;
    
    for (int i = 0; i < n; i++) {
        double rectified = fabs(samples[i]);
        sum_squares += rectified * rectified;
    }
    
    double mean_square = sum_squares / n;
    return sqrt(mean_square);
}

int main() {
    // 正弦波：10个采样点（1个完整周期），幅值5V
    double sine_wave[10];
    for (int i = 0; i < 10; i++) {
        sine_wave[i] = 5.0 * sin(2.0 * M_PI * i / 10.0);
    }
    
    // 方波：8个采样点（2个周期），高电平+3V，低电平-3V
    double square_wave[8] = {3.0, 3.0, -3.0, -3.0, 3.0, 3.0, -3.0, -3.0};
    
    // 三角波：12个采样点（1.5个周期），峰值±4V
    double triangle_wave[12];
    for (int i = 0; i < 12; i++) {
        int quarter = i % 4;
        if (quarter == 0) {
            triangle_wave[i] = 4.0 * (i % 8) / 4.0;
        } else if (quarter == 1) {
            triangle_wave[i] = 4.0 - 8.0 * ((i % 8) - 4) / 4.0;
        } else if (quarter == 2) {
            triangle_wave[i] = -4.0 * ((i % 8) - 4) / 4.0;
        } else {
            triangle_wave[i] = -4.0 + 8.0 * ((i % 8) - 8) / 4.0;
        }
    }
    
    double rms_sine = calculate_true_rms(sine_wave, 10);
    double rms_square = calculate_true_rms(square_wave, 8);
    double rms_triangle = calculate_true_rms(triangle_wave, 12);
    
    printf("正弦波: 真有效值 = %.3f V\n", rms_sine);
    printf("方波: 真有效值 = %.3f V\n", rms_square);
    printf("三角波: 真有效值 = %.3f V\n", rms_triangle);
    
    return 0;
}