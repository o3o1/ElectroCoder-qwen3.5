#include <stdio.h>
#include <math.h>

#define PI 3.14159265358979323846
#define KP 100.0
#define KI 1000.0
#define DT 0.001
#define INTEGRAL_LIMIT 1000.0

void clarke_transform(float a, float b, float c, float *alpha, float *beta) {
    *alpha = a;
    *beta = (b - c) / sqrtf(3.0f);
}

void park_transform(float alpha, float beta, float theta, float *d, float *q) {
    float cos_theta = cosf(theta);
    float sin_theta = sinf(theta);
    *d = alpha * cos_theta + beta * sin_theta;
    *q = -alpha * sin_theta + beta * cos_theta;
}

float pi_controller(float error, float kp, float ki, float *integral) {
    *integral += error * DT;
    if (*integral > INTEGRAL_LIMIT) *integral = INTEGRAL_LIMIT;
    if (*integral < -INTEGRAL_LIMIT) *integral = -INTEGRAL_LIMIT;
    return kp * error + ki * (*integral);
}

void pll_update(float va, float vb, float vc, float *theta, float *frequency, float dt) {
    static float integral = 0.0f;
    
    float alpha, beta;
    clarke_transform(va, vb, vc, &alpha, &beta);
    
    float d, q;
    park_transform(alpha, beta, *theta, &d, &q);
    
    float error = -q;
    float delta_freq = pi_controller(error, KP, KI, &integral);
    
    *frequency = 50.0f + delta_freq;
    *theta += 2.0f * PI * (*frequency) * dt;
    while (*theta > 2.0f * PI) *theta -= 2.0f * PI;
    while (*theta < 0) *theta += 2.0f * PI;
}

int main() {
    float theta_grid = 0.0f;
    float theta_pll = 0.0f;
    float freq_pll = 50.0f;
    
    for (int i = 0; i < 5; i++) {
        float va = sinf(theta_grid);
        float vb = sinf(theta_grid - 2.0f * PI / 3.0f);
        float vc = sinf(theta_grid + 2.0f * PI / 3.0f);
        
        pll_update(va, vb, vc, &theta_pll, &freq_pll, DT);
        
        printf("Step %d: theta_grid=%.6f, theta_pll=%.6f, freq=%.6f\n", 
               i+1, theta_grid, theta_pll, freq_pll);
        
        theta_grid += 0.1f;
    }
    
    return 0;
}