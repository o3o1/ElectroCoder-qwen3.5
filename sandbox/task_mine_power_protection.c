#include <stdio.h>

#define VOLTAGE_REG   (*((volatile float*)0x40000000))
#define TEMP_REG      (*((volatile float*)0x40000004))
#define POWER_CTRL_REG (*((volatile int*)0x40000008))
#define ALARM_CTRL_REG (*((volatile int*)0x4000000C))

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
    
    // Test 1: Normal condition
    VOLTAGE_REG = 30.0f;
    TEMP_REG = 70.0f;
    protection_control(VOLTAGE_REG, TEMP_REG, &power_state, &alarm_state);
    POWER_CTRL_REG = power_state;
    ALARM_CTRL_REG = alarm_state;
    printf("Test 1 - Normal condition: Power state=%d, Alarm state=%d\n", power_state, alarm_state);
    
    // Test 2: Overvoltage trigger
    VOLTAGE_REG = 40.0f;
    TEMP_REG = 75.0f;
    protection_control(VOLTAGE_REG, TEMP_REG, &power_state, &alarm_state);
    POWER_CTRL_REG = power_state;
    ALARM_CTRL_REG = alarm_state;
    printf("Test 2 - Overvoltage trigger: Power state=%d, Alarm state=%d\n", power_state, alarm_state);
    
    // Test 3: Overtemperature trigger
    VOLTAGE_REG = 35.0f;
    TEMP_REG = 90.0f;
    protection_control(VOLTAGE_REG, TEMP_REG, &power_state, &alarm_state);
    POWER_CTRL_REG = power_state;
    ALARM_CTRL_REG = alarm_state;
    printf("Test 3 - Overtemperature trigger: Power state=%d, Alarm state=%d\n", power_state, alarm_state);
    
    return 0;
}