#include <stdio.h>

typedef struct {
    double load_current;
    double grid_voltage;
    double ref_voltage;
    double comp_voltage;
} UPQC_Params;

void calculate_compensation(UPQC_Params *params) {
    params->comp_voltage = params->ref_voltage - params->grid_voltage;
}

int main() {
    UPQC_Params test1 = {15.2, 215.0, 220.0, 0.0};
    UPQC_Params test2 = {8.7, 225.0, 220.0, 0.0};
    UPQC_Params test3 = {12.5, 220.0, 220.0, 0.0};
    
    calculate_compensation(&test1);
    calculate_compensation(&test2);
    calculate_compensation(&test3);
    
    printf("--- Test Case 1 ---\n");
    printf("Load Current: %.1f A\n", test1.load_current);
    printf("Grid Voltage: %.1f V\n", test1.grid_voltage);
    printf("Ref Voltage: %.1f V\n", test1.ref_voltage);
    printf("Compensation Voltage: %.1f V\n\n", test1.comp_voltage);
    
    printf("--- Test Case 2 ---\n");
    printf("Load Current: %.1f A\n", test2.load_current);
    printf("Grid Voltage: %.1f V\n", test2.grid_voltage);
    printf("Ref Voltage: %.1f V\n", test2.ref_voltage);
    printf("Compensation Voltage: %.1f V\n\n", test2.comp_voltage);
    
    printf("--- Test Case 3 ---\n");
    printf("Load Current: %.1f A\n", test3.load_current);
    printf("Grid Voltage: %.1f V\n", test3.grid_voltage);
    printf("Ref Voltage: %.1f V\n", test3.ref_voltage);
    printf("Compensation Voltage: %.1f V\n", test3.comp_voltage);
    
    return 0;
}