#include <stdio.h>
#include <math.h>
#include <stdint.h>

#define PI 3.141592653589793
#define TWO_PI (2.0 * PI)

double taylor_sin(double x) {
    double sign = 1.0;
    
    if (x < 0) {
        x = -x;
        sign = -1.0;
    }
    
    x = fmod(x, TWO_PI);
    
    if (x > PI) {
        x -= PI;
        sign = -sign;
    }
    
    if (x > PI / 2.0) {
        x = PI - x;
    }
    
    double x2 = x * x;
    double x3 = x2 * x;
    double x5 = x3 * x2;
    
    return sign * (x - x3 / 6.0 + x5 / 120.0);
}

uint16_t spwm_sample(double t, double freq, double amplitude) {
    double angle = TWO_PI * freq * t;
    double sin_val = taylor_sin(angle);
    double normalized = (sin_val + 1.0) / 2.0;
    double scaled = normalized * amplitude * 10000.0;
    
    if (scaled < 0.0) return 0;
    if (scaled > 10000.0) return 10000;
    
    return (uint16_t)(scaled + 0.5);
}

int main() {
    printf("=== 泰勒级数近似验证 ===\n");
    double test_angles[] = {0.0, 0.5236, 1.0472, 1.5708};
    const char* labels[] = {"0.0", "π/6", "π/3", "π/2"};
    
    for (int i = 0; i < 4; i++) {
        double x = test_angles[i];
        double taylor_val = taylor_sin(x);
        double std_val = sin(x);
        double error = fabs(taylor_val - std_val);
        printf("x=%s: 泰勒近似=%.6f, 标准sin=%.6f, 误差=%.6f\n", 
               labels[i], taylor_val, std_val, error);
    }
    
    printf("\n=== SPWM采样验证 ===\n");
    double freq = 50.0;
    double amplitude = 0.8;
    double test_times[] = {0.0, 0.005, 0.01};
    
    for (int i = 0; i < 3; i++) {
        double t = test_times[i];
        uint16_t sample = spwm_sample(t, freq, amplitude);
        printf("t=%.3f秒: 量化值=%u\n", t, sample);
    }
    
    printf("\n=== 完整SPWM周期模拟 ===\n");
    freq = 50.0;
    amplitude = 0.8;
    double dt = 0.0001;
    int num_samples = 100;
    uint16_t samples[100];
    
    for (int i = 0; i < num_samples; i++) {
        double t = i * dt;
        samples[i] = spwm_sample(t, freq, amplitude);
    }
    
    printf("前5个采样点:\n");
    for (int i = 0; i < 5; i++) {
        printf("t=%.4f秒: 量化值=%u\n", i * dt, samples[i]);
    }
    
    return 0;
}