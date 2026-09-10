#include <stdio.h>

typedef struct {
    float dc_voltage;
    float output_current;
    float temperature;
    int fault_flag;
} InverterStatus;

int check_fault_protection(InverterStatus *inv) {
    if (inv->dc_voltage > 800.0f) {
        inv->fault_flag = 1;
        return 1;
    }
    if (inv->dc_voltage < 450.0f) {
        inv->fault_flag = 1;
        return 2;
    }
    if (inv->output_current > 20.0f) {
        inv->fault_flag = 1;
        return 3;
    }
    if (inv->temperature > 85.0f) {
        inv->fault_flag = 1;
        return 4;
    }
    inv->fault_flag = 0;
    return 0;
}

int main() {
    InverterStatus test1 = {500.0f, 15.0f, 75.0f, 0};
    InverterStatus test2 = {900.0f, 18.0f, 70.0f, 0};
    InverterStatus test3 = {600.0f, 25.0f, 90.0f, 0};
    
    int result1 = check_fault_protection(&test1);
    int result2 = check_fault_protection(&test2);
    int result3 = check_fault_protection(&test3);
    
    printf("Test 1: Voltage=%.0fV, Current=%.0fA, Temperature=%.0fC -> Status:", 
           test1.dc_voltage, test1.output_current, test1.temperature);
    if (result1 == 0) {
        printf("Normal\n");
    } else if (result1 == 1) {
        printf("Fault(Overvoltage)\n");
    } else if (result1 == 2) {
        printf("Fault(Undervoltage)\n");
    } else if (result1 == 3) {
        printf("Fault(Overcurrent)\n");
    } else if (result1 == 4) {
        printf("Fault(Overtemperature)\n");
    }
    
    printf("Test 2: Voltage=%.0fV, Current=%.0fA, Temperature=%.0fC -> Status:", 
           test2.dc_voltage, test2.output_current, test2.temperature);
    if (result2 == 0) {
        printf("Normal\n");
    } else if (result2 == 1) {
        printf("Fault(Overvoltage)\n");
    } else if (result2 == 2) {
        printf("Fault(Undervoltage)\n");
    } else if (result2 == 3) {
        printf("Fault(Overcurrent)\n");
    } else if (result2 == 4) {
        printf("Fault(Overtemperature)\n");
    }
    
    printf("Test 3: Voltage=%.0fV, Current=%.0fA, Temperature=%.0fC -> Status:", 
           test3.dc_voltage, test3.output_current, test3.temperature);
    if (result3 == 0) {
        printf("Normal\n");
    } else if (result3 == 1) {
        printf("Fault(Overvoltage)\n");
    } else if (result3 == 2) {
        printf("Fault(Undervoltage)\n");
    } else if (result3 == 3) {
        printf("Fault(Overcurrent)\n");
    } else if (result3 == 4) {
        printf("Fault(Overtemperature)\n");
    }
    
    return 0;
}