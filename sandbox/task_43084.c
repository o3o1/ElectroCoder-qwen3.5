#include <stdio.h>
#include <math.h>

#define PI 3.14159265358979323846

void calculate_chebyshev_coeffs(double fc, double fs, double Rp, double *b, double *a) {
    double wp = 2.0 * PI * fc;
    double T = 1.0 / fs;
    
    double epsilon = sqrt(pow(10.0, Rp/10.0) - 1.0);
    
    double beta = asinh(1.0/epsilon) / 2.0;
    double eta = sinh(beta);
    double xi = cosh(beta);
    
    double wc = 2.0 / T * tan(wp * T / 2.0);
    
    double s1_real = -eta * wc;
    double s1_imag = xi * wc;
    double s2_real = -eta * wc;
    double s2_imag = -xi * wc;
    
    double z1_real = (1.0 + s1_real * T/2.0) / (1.0 - s1_real * T/2.0);
    double z1_imag = (s1_imag * T/2.0) / (1.0 - s1_real * T/2.0);
    double z2_real = (1.0 + s2_real * T/2.0) / (1.0 - s2_real * T/2.0);
    double z2_imag = (s2_imag * T/2.0) / (1.0 - s2_real * T/2.0);
    
    double z1_mag_sq = z1_real*z1_real + z1_imag*z1_imag;
    double z2_mag_sq = z2_real*z2_real + z2_imag*z2_imag;
    
    b[0] = 1.0;
    b[1] = -2.0 * z1_real;
    b[2] = z1_mag_sq;
    
    a[0] = 1.0;
    a[1] = -2.0 * z2_real;
    a[2] = z2_mag_sq;
    
    double norm = a[0];
    b[0] /= norm;
    b[1] /= norm;
    b[2] /= norm;
    a[1] /= norm;
    a[2] /= norm;
}

double chebyshev_filter(double input, double *state, const double *coeff) {
    double b0 = coeff[0];
    double b1 = coeff[1];
    double b2 = coeff[2];
    double a1 = coeff[3];
    double a2 = coeff[4];
    
    double w0 = input - a1 * state[0] - a2 * state[1];
    double output = b0 * w0 + b1 * state[0] + b2 * state[1];
    
    state[1] = state[0];
    state[0] = w0;
    
    return output;
}

int main() {
    double fc = 100.0;
    double fs = 1000.0;
    double Rp = 1.0;
    
    double b[3], a[3];
    calculate_chebyshev_coeffs(fc, fs, Rp, b, a);
    
    printf("Chebyshev Type I Lowpass Filter Coefficients:\n");
    printf("b0 = %.6f, b1 = %.6f, b2 = %.6f\n", b[0], b[1], b[2]);
    printf("a0 = 1.000000, a1 = %.6f, a2 = %.6f\n", a[1], a[2]);
    
    double coeff[5] = {b[0], b[1], b[2], a[1], a[2]};
    double state[2] = {0.0, 0.0};
    
    printf("\nTest Signal (50Hz + 150Hz sine waves):\n");
    printf("Input samples and filtered output:\n");
    
    for (int i = 0; i < 10; i++) {
        double t = i / fs;
        double input = sin(2.0 * PI * 50.0 * t) + sin(2.0 * PI * 150.0 * t);
        double output = chebyshev_filter(input, state, coeff);
        
        printf("Sample %d: Input = %.6f, Output = %.6f\n", i, input, output);
    }
    
    return 0;
}