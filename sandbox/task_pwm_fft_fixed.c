#include <stdio.h>
#include <math.h>

#define PI 3.14159265358979323846
#define N 64
#define SAMPLING_FREQ 10000.0
#define SIGNAL_FREQ 1000.0

typedef struct {
    double real;
    double imag;
} Complex;

void generate_pwm_samples(double samples[], double duty_cycle) {
    double period = 1.0 / SIGNAL_FREQ;
    double sample_time = 1.0 / SAMPLING_FREQ;
    double on_time = period * duty_cycle;
    
    for (int i = 0; i < N; i++) {
        double t = i * sample_time;
        double mod_t = fmod(t, period);
        samples[i] = (mod_t < on_time) ? 1.0 : 0.0;
    }
}

Complex complex_add(Complex a, Complex b) {
    Complex result;
    result.real = a.real + b.real;
    result.imag = a.imag + b.imag;
    return result;
}

Complex complex_sub(Complex a, Complex b) {
    Complex result;
    result.real = a.real - b.real;
    result.imag = a.imag - b.imag;
    return result;
}

Complex complex_mul(Complex a, Complex b) {
    Complex result;
    result.real = a.real * b.real - a.imag * b.imag;
    result.imag = a.real * b.imag + a.imag * b.real;
    return result;
}

Complex complex_exp(double angle) {
    Complex result;
    result.real = cos(angle);
    result.imag = -sin(angle);
    return result;
}

double complex_abs(Complex a) {
    return sqrt(a.real * a.real + a.imag * a.imag);
}

void fft(Complex x[], int n) {
    if (n <= 1) return;
    
    Complex even[n/2];
    Complex odd[n/2];
    
    for (int i = 0; i < n/2; i++) {
        even[i] = x[2*i];
        odd[i] = x[2*i + 1];
    }
    
    fft(even, n/2);
    fft(odd, n/2);
    
    for (int k = 0; k < n/2; k++) {
        Complex t = complex_mul(complex_exp(2.0 * PI * k / n), odd[k]);
        x[k] = complex_add(even[k], t);
        x[k + n/2] = complex_sub(even[k], t);
    }
}

void calculate_harmonics(Complex fft_result[], double harmonics[], int num_harmonics) {
    for (int i = 0; i < num_harmonics; i++) {
        harmonics[i] = 2.0 * complex_abs(fft_result[i]) / N;
    }
}

int main() {
    double samples[N];
    Complex fft_data[N];
    double harmonics[10];
    
    printf("PWM FFT Harmonic Analysis\n");
    printf("Sampling Frequency: %.0f Hz\n", SAMPLING_FREQ);
    printf("Signal Frequency: %.0f Hz\n", SIGNAL_FREQ);
    printf("FFT Length: %d points\n\n", N);
    
    printf("Duty 50%% harmonics:\n");
    generate_pwm_samples(samples, 0.5);
    
    for (int i = 0; i < N; i++) {
        fft_data[i].real = samples[i];
        fft_data[i].imag = 0.0;
    }
    
    fft(fft_data, N);
    calculate_harmonics(fft_data, harmonics, 10);
    
    for (int i = 0; i < 5; i++) {
        printf("%dst: %.3f V\n", i+1, harmonics[i]);
    }
    
    printf("\nDuty 25%% harmonics:\n");
    generate_pwm_samples(samples, 0.25);
    
    for (int i = 0; i < N; i++) {
        fft_data[i].real = samples[i];
        fft_data[i].imag = 0.0;
    }
    
    fft(fft_data, N);
    calculate_harmonics(fft_data, harmonics, 10);
    
    for (int i = 0; i < 5; i++) {
        printf("%dst: %.3f V\n", i+1, harmonics[i]);
    }
    
    printf("\nHarmonic comparison (50%% vs 25%% duty):\n");
    printf("Harmonic\t50%% Duty\t25%% Duty\n");
    printf("----------------------------------------\n");
    
    generate_pwm_samples(samples, 0.5);
    for (int i = 0; i < N; i++) {
        fft_data[i].real = samples[i];
        fft_data[i].imag = 0.0;
    }
    fft(fft_data, N);
    calculate_harmonics(fft_data, harmonics, 5);
    double harmonics_50[5];
    for (int i = 0; i < 5; i++) harmonics_50[i] = harmonics[i];
    
    generate_pwm_samples(samples, 0.25);
    for (int i = 0; i < N; i++) {
        fft_data[i].real = samples[i];
        fft_data[i].imag = 0.0;
    }
    fft(fft_data, N);
    calculate_harmonics(fft_data, harmonics, 5);
    double harmonics_25[5];
    for (int i = 0; i < 5; i++) harmonics_25[i] = harmonics[i];
    
    for (int i = 0; i < 5; i++) {
        printf("%dst\t\t%.3f V\t\t%.3f V\n", i+1, harmonics_50[i], harmonics_25[i]);
    }
    
    return 0;
}