#include <stdio.h>

void brake_resistor_control(float bus_voltage, float bus_voltage_max, float *resistor_current, float *energy_joules) {
    if (bus_voltage > bus_voltage_max) {
        *resistor_current = (bus_voltage - bus_voltage_max) * 0.5f;
    } else {
        *resistor_current = 0.0f;
    }
    
    *energy_joules += (*resistor_current) * bus_voltage * 0.01f;
}

int main() {
    float test_voltages[] = {750.0f, 770.0f, 790.0f, 810.0f, 820.0f, 810.0f, 790.0f, 770.0f};
    int num_tests = sizeof(test_voltages) / sizeof(test_voltages[0]);
    float bus_voltage_max = 800.0f;
    float resistor_current = 0.0f;
    float total_energy = 0.0f;
    
    printf("Inverter Brake Resistor Control Simulation\n");
    printf("Maximum bus voltage: %.1fV\n\n", bus_voltage_max);
    
    for (int i = 0; i < num_tests; i++) {
        brake_resistor_control(test_voltages[i], bus_voltage_max, &resistor_current, &total_energy);
        printf("Voltage: %.1fV, Brake current: %.2fA, Total energy: %.2fJ\n", 
               test_voltages[i], resistor_current, total_energy);
    }
    
    return 0;
}