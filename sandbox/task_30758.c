#include <stdio.h>
#include <math.h>

double calculate_THD(double fundamental_amplitude, double harmonics[], int harmonics_count) {
    double sum_squares = 0.0;
    
    for (int i = 0; i < harmonics_count; i++) {
        sum_squares += harmonics[i] * harmonics[i];
    }
    
    if (fundamental_amplitude == 0.0) {
        return 0.0;
    }
    
    return sqrt(sum_squares) / fundamental_amplitude * 100.0;
}

int main() {
    // Test case 1
    double harmonics1[] = {1.2, 0.8, 0.3};
    double thd1 = calculate_THD(10.0, harmonics1, 3);
    printf("Test case 1: THD = %.2f%%\n", thd1);
    
    // Test case 2
    double harmonics2[] = {0.5, 0.2};
    double thd2 = calculate_THD(5.0, harmonics2, 2);
    printf("Test case 2: THD = %.2f%%\n", thd2);
    
    // Test case 3
    double harmonics3[] = {};
    double thd3 = calculate_THD(8.0, harmonics3, 0);
    printf("Test case 3: THD = %.2f%%\n", thd3);
    
    return 0;
}