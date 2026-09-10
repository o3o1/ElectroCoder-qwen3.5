#include <stdio.h>
#include <math.h>

double trapezoidal_integral(const double *samples, int n, double dt) {
    if (n <= 1) return 0.0;
    
    double sum = 0.5 * samples[0] + 0.5 * samples[n-1];
    for (int i = 1; i < n-1; i++) {
        sum += samples[i];
    }
    return sum * dt;
}

int main() {
    printf("Trapezoidal Integration Test\n");
    printf("============================\n\n");
    
    // Test case 1: Constant signal f(t)=2.0
    {
        const int n = 10;
        double dt = 0.1;
        double samples[n];
        for (int i = 0; i < n; i++) {
            samples[i] = 2.0;
        }
        double result = trapezoidal_integral(samples, n, dt);
        double expected = 2.0;
        double error = fabs(result - expected);
        
        printf("Test 1: Constant signal f(t)=2.0\n");
        printf("  Samples: %d, dt=%.2f\n", n, dt);
        printf("  Computed: %.6f\n", result);
        printf("  Expected: %.6f\n", expected);
        printf("  Absolute error: %.6e\n\n", error);
    }
    
    // Test case 2: Ramp signal f(t)=t
    {
        const int n = 5;
        double dt = 0.5;
        double samples[n];
        for (int i = 0; i < n; i++) {
            samples[i] = i * dt;  // t starts from 0
        }
        double result = trapezoidal_integral(samples, n, dt);
        double expected = 2.5;  // ∫₀²t dt = 0.5*t²|₀² = 2.0, but sampling to t=2.0
        double error = fabs(result - expected);
        
        printf("Test 2: Ramp signal f(t)=t\n");
        printf("  Samples: %d, dt=%.2f\n", n, dt);
        printf("  Computed: %.6f\n", result);
        printf("  Expected: %.6f\n", expected);
        printf("  Absolute error: %.6e\n\n", error);
    }
    
    // Test case 3: Sine signal f(t)=sin(t)
    {
        const int n = 4;
        double dt = 3.14159265358979323846 / 6.0;  // π/6
        double samples[n];
        for (int i = 0; i < n; i++) {
            samples[i] = sin(i * dt);
        }
        double result = trapezoidal_integral(samples, n, dt);
        double expected = 0.8966;  // ∫₀^(π/2) sin(t) dt = 1 - cos(π/2) = 1.0
        double error = fabs(result - expected);
        
        printf("Test 3: Sine signal f(t)=sin(t)\n");
        printf("  Samples: %d, dt=π/6≈%.6f\n", n, dt);
        printf("  Computed: %.6f\n", result);
        printf("  Expected: %.6f\n", expected);
        printf("  Absolute error: %.6e\n", error);
    }
    
    return 0;
}