#include <stdio.h>

double soc_ah_integration(double initial_soc, double capacity_ah, double current_a[], double time_s[], int n) {
    double soc = initial_soc;
    
    for (int i = 0; i < n; i++) {
        double delta_soc = (current_a[i] * time_s[i] / (capacity_ah * 3600.0)) * 100.0;
        soc += delta_soc;
        
        if (soc > 100.0) soc = 100.0;
        if (soc < 0.0) soc = 0.0;
    }
    
    return soc;
}

int main() {
    printf("=== Battery SoC AH Integration Estimation Test ===\n\n");
    
    // Test 1: Constant current discharge
    printf("Test 1: Constant Current Discharge Scenario\n");
    double initial_soc1 = 100.0;
    double capacity1 = 50.0;
    
    double current1[] = {10.0, 10.0, 10.0, 10.0, 10.0, 10.0};
    double time1[] = {600.0, 600.0, 600.0, 600.0, 600.0, 600.0};
    int n1 = 6;
    
    printf("Initial SoC: %.2f%%\n", initial_soc1);
    printf("Battery Capacity: %.2f Ah\n", capacity1);
    printf("Current Array: ");
    for (int i = 0; i < n1; i++) printf("%.1fA ", current1[i]);
    printf("\nTime Array: ");
    for (int i = 0; i < n1; i++) printf("%.0fs ", time1[i]);
    
    double soc1 = soc_ah_integration(initial_soc1, capacity1, current1, time1, n1);
    printf("\nEstimated SoC: %.2f%%\n", soc1);
    printf("Theoretical Value: 80.00%%\n\n");
    
    // Test 2: Charge-discharge alternating scenario
    printf("Test 2: Charge-Discharge Alternating Scenario\n");
    double initial_soc2 = 50.0;
    double capacity2 = 30.0;
    
    double current2[] = {5.0, -8.0, 3.0};
    double time2[] = {1800.0, 1200.0, 2400.0};
    int n2 = 3;
    
    printf("Initial SoC: %.2f%%\n", initial_soc2);
    printf("Battery Capacity: %.2f Ah\n", capacity2);
    printf("Current Array: ");
    for (int i = 0; i < n2; i++) printf("%.1fA ", current2[i]);
    printf("\nTime Array: ");
    for (int i = 0; i < n2; i++) printf("%.0fs ", time2[i]);
    
    double soc2 = soc_ah_integration(initial_soc2, capacity2, current2, time2, n2);
    printf("\nEstimated SoC: %.2f%%\n", soc2);
    printf("Theoretical Value: 56.11%%\n\n");
    
    // Test 3: Overcharge protection test
    printf("Test 3: Overcharge Protection Test\n");
    double initial_soc3 = 95.0;
    double capacity3 = 20.0;
    double current3[] = {-10.0, -10.0};
    double time3[] = {1800.0, 1800.0};
    int n3 = 2;
    
    double soc3 = soc_ah_integration(initial_soc3, capacity3, current3, time3, n3);
    printf("Initial SoC: %.2f%%, After Charge SoC: %.2f%% (Should be limited to 100%%)\n", initial_soc3, soc3);
    
    // Test 4: Overdischarge protection test
    printf("\nTest 4: Overdischarge Protection Test\n");
    double initial_soc4 = 5.0;
    double capacity4 = 20.0;
    double current4[] = {15.0, 15.0};
    double time4[] = {1800.0, 1800.0};
    int n4 = 2;
    
    double soc4 = soc_ah_integration(initial_soc4, capacity4, current4, time4, n4);
    printf("Initial SoC: %.2f%%, After Discharge SoC: %.2f%% (Should be limited to 0%%)\n", initial_soc4, soc4);
    
    return 0;
}