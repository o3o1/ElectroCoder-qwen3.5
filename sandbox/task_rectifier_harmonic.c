#include <stdio.h>
#include <math.h>

#define PI 3.14159265358979323846
#define TS 0.0001

void harmonic_suppression(double *i_load, double *i_compensated, int N, double L, double C, double Kp, double Ki) {
    double f_c = 1.0 / (2.0 * PI * sqrt(L * C));
    double alpha = 2.0 * PI * f_c * TS;
    
    double i_filtered = 0.0;
    double integral = 0.0;
    
    for (int n = 0; n < N; n++) {
        double i_load_n = i_load[n];
        
        i_filtered = alpha * i_load_n + (1.0 - alpha) * i_filtered;
        
        double i_harmonic = i_load_n - i_filtered;
        
        integral += i_harmonic * TS;
        
        double i_comp = Kp * i_harmonic + Ki * integral;
        
        i_compensated[n] = i_load_n - i_comp;
    }
}

int main() {
    int N = 1000;
    double i_load[N];
    double i_compensated[N];
    
    for (int n = 0; n < N; n++) {
        double t = n * TS;
        i_load[n] = sin(2.0 * PI * 50.0 * t) + 0.3 * sin(2.0 * PI * 250.0 * t);
    }
    
    double L = 0.01;
    double C = 1e-4;
    double Kp = 0.5;
    double Ki = 0.1;
    
    harmonic_suppression(i_load, i_compensated, N, L, C, Kp, Ki);
    
    for (int n = 0; n < 10; n++) {
        printf("n=%d, i_load=%.6f, i_compensated=%.6f\n", n, i_load[n], i_compensated[n]);
    }
    
    return 0;
}