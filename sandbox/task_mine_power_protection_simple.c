#include <stdio.h>

// Mock hardware registers
float VOLTAGE_REG = 0;
float TEMP_REG = 0;
int POWER_CTRL_REG = 0;
int ALARM_CTRL_REG = 0;

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
    
    printf("矿山电源防爆控制系统测试\n");
    printf("========================\n");
    
    // 测试1: 正常工况
    VOLTAGE_REG = 30.0f;
    TEMP_REG = 70.0f;
    protection_control(VOLTAGE_REG, TEMP_REG, &power_state, &alarm_state);
    POWER_CTRL_REG = power_state;
    ALARM_CTRL_REG = alarm_state;
    printf("测试1 - 正常工况：电源状态=%d, 报警状态=%d\n", power_state, alarm_state);
    
    // 测试2: 过压触发
    VOLTAGE_REG = 40.0f;
    TEMP_REG = 75.0f;
    protection_control(VOLTAGE_REG, TEMP_REG, &power_state, &alarm_state);
    POWER_CTRL_REG = power_state;
    ALARM_CTRL_REG = alarm_state;
    printf("测试2 - 过压触发：电源状态=%d, 报警状态=%d\n", power_state, alarm_state);
    
    // 测试3: 过温触发
    VOLTAGE_REG = 35.0f;
    TEMP_REG = 90.0f;
    protection_control(VOLTAGE_REG, TEMP_REG, &power_state, &alarm_state);
    POWER_CTRL_REG = power_state;
    ALARM_CTRL_REG = alarm_state;
    printf("测试3 - 过温触发：电源状态=%d, 报警状态=%d\n", power_state, alarm_state);
    
    return 0;
}