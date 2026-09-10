#include <stdio.h>
#include <math.h>

// Hardware register macro definitions
#define PFC_MAX_VOLTAGE 450.0f
#define LLC_MAX_POWER 5000.0f
#define GRID_VOLTAGE_MIN 176.0f
#define GRID_VOLTAGE_MAX 264.0f

// PFC target bus voltage
#define PFC_TARGET_VOLTAGE 400.0f

// LLC charging parameters
#define LLC_CONSTANT_CURRENT 10.0f
#define LLC_CHARGE_MARGIN 0.5f

void obc_control(float *pfc_output, float *llc_output, float grid_voltage, float battery_voltage, float battery_soc) {
    // 1. PFC stage control: Rectify and boost grid voltage to stable 400V DC bus voltage
    // In real applications there would be more complex control algorithms, simplified here as target voltage control
    float pfc_target = PFC_TARGET_VOLTAGE;
    
    // Consider grid voltage range limitations
    if (grid_voltage < GRID_VOLTAGE_MIN) {
        // Grid voltage too low, reduce PFC output voltage
        pfc_target = PFC_TARGET_VOLTAGE * (grid_voltage / GRID_VOLTAGE_MIN);
    } else if (grid_voltage > GRID_VOLTAGE_MAX) {
        // Grid voltage too high, limit PFC output voltage
        pfc_target = PFC_TARGET_VOLTAGE * (GRID_VOLTAGE_MAX / grid_voltage);
    }
    
    // Ensure not exceeding maximum voltage limit
    if (pfc_target > PFC_MAX_VOLTAGE) {
        pfc_target = PFC_MAX_VOLTAGE;
    }
    
    *pfc_output = pfc_target;
    
    // 2. LLC stage control: Dynamically adjust output based on battery SOC
    if (battery_soc < 0.8f) {
        // Constant current charging mode: SOC < 0.8, output current = 10A
        *llc_output = LLC_CONSTANT_CURRENT;
    } else {
        // Constant voltage charging mode: SOC ≥ 0.8, output voltage = battery voltage + 0.5V
        // Here llc_output represents output voltage in constant voltage mode
        float target_voltage = battery_voltage + LLC_CHARGE_MARGIN;
        
        // Calculate corresponding current (based on power limit)
        float max_current = LLC_MAX_POWER / target_voltage;
        
        // In constant voltage mode, output current is determined by load, here negative value indicates constant voltage mode
        *llc_output = -target_voltage; // Negative value indicates constant voltage mode
    }
}

int main() {
    printf("Electric Vehicle On-Board Charger (OBC) Control Algorithm Test\n");
    printf("==============================================================\n\n");
    
    // Test case a: Normal charging scenario
    {
        float grid_voltage = 220.0f;
        float battery_voltage = 360.0f;
        float battery_soc = 0.6f;
        float pfc_output, llc_output;
        
        obc_control(&pfc_output, &llc_output, grid_voltage, battery_voltage, battery_soc);
        
        printf("Test Case a - Normal Charging Scenario:\n");
        printf("  Grid Voltage: %.1fV\n", grid_voltage);
        printf("  Battery Voltage: %.1fV, SOC: %.1f%%\n", battery_voltage, battery_soc * 100);
        printf("  PFC Output Voltage: %.1fV\n", pfc_output);
        if (llc_output >= 0) {
            printf("  LLC Output Current: %.1fA (Constant Current Mode)\n", llc_output);
        } else {
            printf("  LLC Output Voltage: %.1fV (Constant Voltage Mode)\n", -llc_output);
        }
        printf("\n");
    }
    
    // Test case b: Constant voltage charging scenario
    {
        float grid_voltage = 230.0f;
        float battery_voltage = 400.0f;
        float battery_soc = 0.9f;
        float pfc_output, llc_output;
        
        obc_control(&pfc_output, &llc_output, grid_voltage, battery_voltage, battery_soc);
        
        printf("Test Case b - Constant Voltage Charging Scenario:\n");
        printf("  Grid Voltage: %.1fV\n", grid_voltage);
        printf("  Battery Voltage: %.1fV, SOC: %.1f%%\n", battery_voltage, battery_soc * 100);
        printf("  PFC Output Voltage: %.1fV\n", pfc_output);
        if (llc_output >= 0) {
            printf("  LLC Output Current: %.1fA (Constant Current Mode)\n", llc_output);
        } else {
            printf("  LLC Output Voltage: %.1fV (Constant Voltage Mode)\n", -llc_output);
        }
        printf("\n");
    }
    
    // Test case c: Boundary test
    {
        float grid_voltage = 180.0f;
        float battery_voltage = 300.0f;
        float battery_soc = 0.95f;
        float pfc_output, llc_output;
        
        obc_control(&pfc_output, &llc_output, grid_voltage, battery_voltage, battery_soc);
        
        printf("Test Case c - Boundary Test (Low Grid Voltage):\n");
        printf("  Grid Voltage: %.1fV\n", grid_voltage);
        printf("  Battery Voltage: %.1fV, SOC: %.1f%%\n", battery_voltage, battery_soc * 100);
        printf("  PFC Output Voltage: %.1fV\n", pfc_output);
        if (llc_output >= 0) {
            printf("  LLC Output Current: %.1fA (Constant Current Mode)\n", llc_output);
        } else {
            printf("  LLC Output Voltage: %.1fV (Constant Voltage Mode)\n", -llc_output);
        }
        printf("\n");
    }
    
    // Additional test: High grid voltage scenario
    {
        float grid_voltage = 280.0f;
        float battery_voltage = 380.0f;
        float battery_soc = 0.7f;
        float pfc_output, llc_output;
        
        obc_control(&pfc_output, &llc_output, grid_voltage, battery_voltage, battery_soc);
        
        printf("Additional Test - High Grid Voltage:\n");
        printf("  Grid Voltage: %.1fV\n", grid_voltage);
        printf("  Battery Voltage: %.1fV, SOC: %.1f%%\n", battery_voltage, battery_soc * 100);
        printf("  PFC Output Voltage: %.1fV\n", pfc_output);
        if (llc_output >= 0) {
            printf("  LLC Output Current: %.1fA (Constant Current Mode)\n", llc_output);
        } else {
            printf("  LLC Output Voltage: %.1fV (Constant Voltage Mode)\n", -llc_output);
        }
    }
    
    return 0;
}