#include <stdio.h>
#include <math.h>

#define SWITCHING_FREQ 100000
#define NUM_PHASES_MAX 6

double calculate_ripple_current(double I_pp, int N, double D) {
    if (D == 0.0 || D == 1.0) {
        return 0.0;
    }
    
    if (N <= 0) {
        return I_pp;
    }
    
    double k = fabs(sin(N * M_PI * D)) / (N * sin(M_PI * D));
    return I_pp * k;
}

double phase_delay_calculation(int N, double T_sw, int i) {
    if (N <= 0 || i < 0 || i >= N) {
        return 0.0;
    }
    return i * T_sw / N;
}

int main() {
    struct TestCase {
        int N;
        double D;
        double I_pp;
        double T_sw;
    } test_cases[] = {
        {3, 0.4, 2.0, 1.0e-5},
        {4, 0.6, 1.5, 2.0e-5},
        {2, 0.3, 3.0, 5.0e-6}
    };
    
    int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);
    
    for (int case_idx = 0; case_idx < num_cases; case_idx++) {
        int N = test_cases[case_idx].N;
        double D = test_cases[case_idx].D;
        double I_pp = test_cases[case_idx].I_pp;
        double T_sw = test_cases[case_idx].T_sw;
        
        double total_ripple = calculate_ripple_current(I_pp, N, D);
        
        printf("N=%d, D=%.2f, I_pp_single=%.1fA => 总纹波: %.3fA, 相位延迟: [", 
               N, D, I_pp, total_ripple);
        
        for (int i = 0; i < N; i++) {
            double delay = phase_delay_calculation(N, T_sw, i);
            printf("%.6fs", delay);
            if (i < N - 1) {
                printf(", ");
            }
        }
        printf("]\n");
    }
    
    return 0;
}