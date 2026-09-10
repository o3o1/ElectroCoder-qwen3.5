#include <stdio.h>
#include <math.h>

// Function to calculate insulation resistance using balanced bridge method
void calculate_insulation_resistance(float Vdc, float Vp, float Vn, float R1, float R2, float* Rp, float* Rn) {
    // Calculate positive insulation resistance
    if (Vp > 0.001f) {  // Avoid division by zero
        *Rp = R1 * (Vdc - Vp) / Vp - R2;
        if (*Rp < 0) *Rp = 0;  // Insulation resistance cannot be negative
    } else {
        *Rp = INFINITY;  // Perfect insulation
    }
    
    // Calculate negative insulation resistance
    if (Vn > 0.001f) {  // Avoid division by zero
        *Rn = R1 * (Vdc - Vn) / Vn - R2;
        if (*Rn < 0) *Rn = 0;  // Insulation resistance cannot be negative
    } else {
        *Rn = INFINITY;  // Perfect insulation
    }
}

int main() {
    // Test cases data
    float test_cases[3][5] = {
        // Vdc, Vp, Vn, R1, R2
        {400.0f, 200.0f, 200.0f, 1000.0f, 1000.0f},  // Test case 1
        {500.0f, 300.0f, 250.0f, 1500.0f, 1200.0f},  // Test case 2
        {600.0f, 400.0f, 350.0f, 2000.0f, 1800.0f}   // Test case 3
    };
    
    printf("DC Charging Pile Insulation Detection - Balanced Bridge Algorithm\n");
    printf("=================================================================\n\n");
    
    // Run three test cases
    for (int i = 0; i < 3; i++) {
        float Vdc = test_cases[i][0];
        float Vp = test_cases[i][1];
        float Vn = test_cases[i][2];
        float R1 = test_cases[i][3];
        float R2 = test_cases[i][4];
        
        // Calculate insulation resistance
        float Rp, Rn;
        calculate_insulation_resistance(Vdc, Vp, Vn, R1, R2, &Rp, &Rn);
        
        // Print results
        printf("Test Case %d: Vdc=%.2fV, Vp=%.2fV, Vn=%.2fV => Rp=%.2fΩ, Rn=%.2fΩ\n", 
               i+1, Vdc, Vp, Vn, Rp, Rn);
    }
    
    // Additional test case with perfect insulation (Vp = Vn = Vdc/2)
    printf("\nAdditional Test: Ideal Insulation Case\n");
    float Vdc_test = 400.0f;
    float Vp_test = 200.0f;
    float Vn_test = 200.0f;
    float R1_test = 1000.0f;
    float R2_test = 1000.0f;
    float Rp_test, Rn_test;
    
    calculate_insulation_resistance(Vdc_test, Vp_test, Vn_test, R1_test, R2_test, &Rp_test, &Rn_test);
    printf("Vdc=%.2fV, Vp=%.2fV, Vn=%.2fV => Rp=%.2fΩ, Rn=%.2fΩ\n", 
           Vdc_test, Vp_test, Vn_test, Rp_test, Rn_test);
    
    return 0;
}