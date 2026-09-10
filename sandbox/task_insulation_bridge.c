#include <stdio.h>
#include <math.h>

// Mock hardware register definitions for ADC simulation
#define ADC_CHANNEL_VP  0
#define ADC_CHANNEL_VN  1
#define ADC_CHANNEL_VDC 2
#define ADC_RESOLUTION  4096
#define ADC_REF_VOLTAGE 5.0f

// Mock ADC read function
float mock_adc_read(int channel) {
    // Simulated ADC values for testing
    static float test_values[3][3] = {
        {200.0f, 200.0f, 400.0f},  // Test case 1
        {300.0f, 250.0f, 500.0f},  // Test case 2
        {400.0f, 350.0f, 600.0f}   // Test case 3
    };
    static int test_case = 0;
    
    // Return simulated value based on channel
    switch(channel) {
        case ADC_CHANNEL_VP: return test_values[test_case][0];
        case ADC_CHANNEL_VN: return test_values[test_case][1];
        case ADC_CHANNEL_VDC: return test_values[test_case][2];
        default: return 0.0f;
    }
}

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
    float test_R1[3] = {1000.0f, 1500.0f, 2000.0f};
    float test_R2[3] = {1000.0f, 1200.0f, 1800.0f};
    
    printf("直流充电桩绝缘检测平衡电桥算法模拟\n");
    printf("=====================================\n\n");
    
    // Run three test cases
    for (int i = 0; i < 3; i++) {
        // Simulate ADC readings
        float Vp = mock_adc_read(ADC_CHANNEL_VP);
        float Vn = mock_adc_read(ADC_CHANNEL_VN);
        float Vdc = mock_adc_read(ADC_CHANNEL_VDC);
        
        // Calculate insulation resistance
        float Rp, Rn;
        calculate_insulation_resistance(Vdc, Vp, Vn, test_R1[i], test_R2[i], &Rp, &Rn);
        
        // Print results
        printf("测试用例%d：Vdc=%.2fV, Vp=%.2fV, Vn=%.2fV => Rp=%.2fΩ, Rn=%.2fΩ\n", 
               i+1, Vdc, Vp, Vn, Rp, Rn);
        
        // Move to next test case
        mock_adc_read(ADC_CHANNEL_VP);  // Increment test case counter
    }
    
    // Additional test case with perfect insulation (Vp = Vn = Vdc/2)
    printf("\n附加测试：理想绝缘情况\n");
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