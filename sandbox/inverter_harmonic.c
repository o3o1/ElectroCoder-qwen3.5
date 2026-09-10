#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define PI 3.14159265358979323846
#define FS 10000.0
#define F1 50.0
#define F3 150.0
#define F5 250.0
#define N 1024

typedef struct {
    double amplitude;
    double phase;
} HarmonicInfo;

HarmonicInfo compute_dft_component(double *signal, int n, double freq) {
    HarmonicInfo result = {0.0, 0.0};
    double real = 0.0;
    double imag = 0.0;
    
    for (int k = 0; k < n; k++) {
        double angle = 2.0 * PI * freq * k / FS;
        real += signal[k] * cos(angle);
        imag += signal[k] * sin(angle);
    }
    
    real = 2.0 * real / n;
    imag = 2.0 * imag / n;
    
    result.amplitude = sqrt(real * real + imag * imag);
    result.phase = atan2(imag, real);
    
    return result;
}

void harmonic_compensation(double *voltage, int n, double *compensated) {
    HarmonicInfo fundamental = compute_dft_component(voltage, n, F1);
    HarmonicInfo harmonic3 = compute_dft_component(voltage, n, F3);
    HarmonicInfo harmonic5 = compute_dft_component(voltage, n, F5);
    
    for (int i = 0; i < n; i++) {
        double t = i / FS;
        
        double h3 = harmonic3.amplitude * cos(2.0 * PI * F3 * t + harmonic3.phase);
        double h5 = harmonic5.amplitude * cos(2.0 * PI * F5 * t + harmonic5.phase);
        
        compensated[i] = voltage[i] - h3 - h5;
    }
}

int main() {
    double voltage[N];
    double compensated[N];
    
    double A1 = 311.0;
    double A3 = 15.5;
    double A5 = 9.3;
    
    double phi1 = 0.0;
    double phi3 = PI/6;
    double phi5 = PI/4;
    
    for (int i = 0; i < N; i++) {
        double t = i / FS;
        
        double v1 = A1 * cos(2.0 * PI * F1 * t + phi1);
        double v3 = A3 * cos(2.0 * PI * F3 * t + phi3);
        double v5 = A5 * cos(2.0 * PI * F5 * t + phi5);
        
        voltage[i] = v1 + v3 + v5;
    }
    
    harmonic_compensation(voltage, N, compensated);
    
    printf("Inverter harmonic compensation results:\n");
    printf("Sampling frequency: %.0f Hz, Samples: %d\n", FS, N);
    printf("Fundamental: %.0f Hz, 3rd harmonic: %.0f Hz, 5th harmonic: %.0f Hz\n\n", F1, F3, F5);
    
    for (int i = 0; i < 10; i++) {
        printf("Sample [%d]: raw=%7.2f V, compensated=%7.2f V\n", 
               i, voltage[i], compensated[i]);
    }
    
    printf("\nHarmonic detection results:\n");
    
    HarmonicInfo fundamental = compute_dft_component(voltage, N, F1);
    HarmonicInfo harmonic3 = compute_dft_component(voltage, N, F3);
    HarmonicInfo harmonic5 = compute_dft_component(voltage, N, F5);
    
    printf("Fundamental(50Hz): amplitude=%7.2f V, phase=%7.2f deg\n", 
           fundamental.amplitude, fundamental.phase * 180.0 / PI);
    printf("3rd harmonic(150Hz): amplitude=%7.2f V, phase=%7.2f deg\n", 
           harmonic3.amplitude, harmonic3.phase * 180.0 / PI);
    printf("5th harmonic(250Hz): amplitude=%7.2f V, phase=%7.2f deg\n", 
           harmonic5.amplitude, harmonic5.phase * 180.0 / PI);
    
    double thd = sqrt(harmonic3.amplitude * harmonic3.amplitude + 
                      harmonic5.amplitude * harmonic5.amplitude) / 
                 fundamental.amplitude * 100.0;
    printf("\nTotal Harmonic Distortion(THD): %.2f%%\n", thd);
    
    return 0;
}