#include <stdio.h>

float calculate_insulation_resistance(float U_plus, float U_minus, float R_bridge) {
    float diff = U_plus - U_minus;
    if (diff < 0.001f && diff > -0.001f) {
        return 1000000.0f;
    }
    return R_bridge * (U_plus + U_minus) / diff;
}

void check_insulation_fault(float R_ins, float threshold) {
    if (R_ins < threshold) {
        printf("ALARM: Insulation fault detected! R_ins = %.2f kΩ\n", R_ins);
    } else {
        printf("OK: Insulation normal. R_ins = %.2f kΩ\n", R_ins);
    }
}

int main() {
    float R_bridge = 10.0f;
    float threshold = 20.0f;
    
    printf("=== Insulation Resistance Fault Detection Test ===\n\n");
    
    printf("Test 1: Normal insulation (balanced system)\n");
    float U_plus1 = 150.0f;
    float U_minus1 = 150.0f;
    float R_ins1 = calculate_insulation_resistance(U_plus1, U_minus1, R_bridge);
    if (R_ins1 > 999999.0f) {
        printf("Perfectly balanced system: R_ins = INFINITY kΩ\n");
        printf("OK: Insulation normal. R_ins = INFINITY kΩ\n");
    } else {
        check_insulation_fault(R_ins1, threshold);
    }
    printf("\n");
    
    printf("Test 2: Good insulation (slight imbalance)\n");
    float U_plus2 = 155.0f;
    float U_minus2 = 145.0f;
    float R_ins2 = calculate_insulation_resistance(U_plus2, U_minus2, R_bridge);
    check_insulation_fault(R_ins2, threshold);
    printf("\n");
    
    printf("Test 3: Insulation fault (low resistance)\n");
    float U_plus3 = 180.0f;
    float U_minus3 = 120.0f;
    float R_ins3 = calculate_insulation_resistance(U_plus3, U_minus3, R_bridge);
    check_insulation_fault(R_ins3, threshold);
    printf("\n");
    
    printf("Test 4: Severe insulation fault\n");
    float U_plus4 = 195.0f;
    float U_minus4 = 105.0f;
    float R_ins4 = calculate_insulation_resistance(U_plus4, U_minus4, R_bridge);
    check_insulation_fault(R_ins4, threshold);
    printf("\n");
    
    printf("Test 5: Edge case (very low voltage difference)\n");
    float U_plus5 = 150.1f;
    float U_minus5 = 149.9f;
    float R_ins5 = calculate_insulation_resistance(U_plus5, U_minus5, R_bridge);
    check_insulation_fault(R_ins5, threshold);
    
    return 0;
}