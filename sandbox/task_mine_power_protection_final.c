#include <stdio.h>

// Mock hardware registers as per requirement
#define VOLTAGE_REG   (*((volatile float*)0x40000000))
#define TEMP_REG      (*((volatile float*)0x40000004))
#define POWER_CTRL_REG (*((volatile int*)0x40000008))
#define ALARM_CTRL_REG (*((volatile int*)0x4000000C))

// Global variables to simulate hardware registers
volatile float mock_voltage_reg = 0;
volatile float mock_temp_reg = 0;
volatile int mock_power_ctrl_reg = 0;
volatile int mock_alarm_ctrl_reg = 0;

// Redefine macros to use mock variables
#undef VOLTAGE_REG
#undef TEMP_REG
#undef POWER_CTRL_REG
#undef ALARM_CTRL_REG
#define VOLTAGE_REG   mock_voltage_reg
#define TEMP_REG      mock_temp_reg
#define POWER_CTRL_REG mock_power_ctrl_reg
#define ALARM_CTRL_REG mock_alarm_ctrl_reg

void protection_control(float voltage, float temperature, int *power_state, int *alarm_state) {
    if (voltage > 36.0f || temperature > 85.0f) {
        *power_state = 0;
        *alarm_state = 1;
    } else {
        *power_state = 1;
        *alarm_state = 0;
    }
}

int main() {
    int power_state, alarm_state;
    
    printf("Mine Power Protection System Test\n");
    printf("=================================\n");
    
    // Test 1: Normal condition - voltage 30V, temperature 70C
    VOLTAGE_REG = 30.0f;
    TEMP_REG = 70.0f;
    protection_control(VOLTAGE_REG, TEMP_REG, &power_state, &alarm_state);
    POWER_CTRL_REG = power_state;
    ALARM_CTRL_REG = alarm_state;
    printf("Test 1 - Normal condition: Power state=%d, Alarm state=%d\n", power_state, alarm_state);
    
    // Test 2: Overvoltage trigger - voltage 40V, temperature 75C
    VOLTAGE_REG = 40.0f;
    TEMP_REG = 75.0f;
    protection_control(VOLTAGE_REG, TEMP_REG, &power_state, &alarm_state);
    POWER_CTRL_REG = power_state;
    ALARM_CTRL_REG = alarm_state;
    printf("Test 2 - Overvoltage trigger: Power state=%d, Alarm state=%d\n", power_state, alarm_state);
    
    // Test 3: Overtemperature trigger - voltage 35V, temperature 90C
    VOLTAGE_REG = 35.0f;
    TEMP_REG = 90.0f;
    protection_control(VOLTAGE_REG, TEMP_REG, &power_state, &alarm_state);
    POWER_CTRL_REG = power_state;
    ALARM_CTRL_REG = alarm_state;
    printf("Test 3 - Overtemperature trigger: Power state=%d, Alarm state=%d\n", power_state, alarm_state);
    
    return 0;
}