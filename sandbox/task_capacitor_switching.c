#include <stdio.h>
#include <math.h>

int calculate_capacitor_groups(double Q_load, double Qc) {
    if (Q_load <= 0.0) {
        return 0;
    }
    
    int max_groups = 4;
    int best_groups = 0;
    double best_remainder = Q_load;
    
    for (int n = 0; n <= max_groups; n++) {
        double Q_remain = Q_load - n * Qc;
        
        if (Q_remain <= 0.0) {
            if (fabs(Q_remain) < fabs(best_remainder)) {
                best_remainder = Q_remain;
                best_groups = n;
            }
        }
    }
    
    return best_groups;
}

int main() {
    double test_cases[][2] = {
        {150.0, 50.0},
        {80.0, 30.0},
        {-20.0, 40.0},
        {250.0, 60.0}
    };
    
    for (int i = 0; i < 4; i++) {
        double Q_load = test_cases[i][0];
        double Qc = test_cases[i][1];
        int groups = calculate_capacitor_groups(Q_load, Qc);
        printf("Q_load=%.1f, Qc=%.1f => groups=%d\n", Q_load, Qc, groups);
    }
    
    return 0;
}