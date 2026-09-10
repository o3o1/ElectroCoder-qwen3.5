#include <stdio.h>
#include <stdbool.h>

#define OVER_VOLTAGE_THRESHOLD 450.0f
#define UNDER_VOLTAGE_THRESHOLD 300.0f
#define OVER_CURRENT_THRESHOLD 20.0f
#define OVER_TEMP_THRESHOLD 85.0f

typedef enum {
    FAULT_NORMAL = 0,
    FAULT_OVER_VOLTAGE = 1,
    FAULT_OVER_CURRENT = 2,
    FAULT_OVER_TEMP = 3,
    FAULT_UNDER_VOLTAGE = 4
} FaultCode;

void inverter_protection_check(float voltage, float current, float temperature, int* fault_code) {
    if (voltage > OVER_VOLTAGE_THRESHOLD) {
        *fault_code = FAULT_OVER_VOLTAGE;
        return;
    }
    
    if (current > OVER_CURRENT_THRESHOLD) {
        *fault_code = FAULT_OVER_CURRENT;
        return;
    }
    
    if (temperature > OVER_TEMP_THRESHOLD) {
        *fault_code = FAULT_OVER_TEMP;
        return;
    }
    
    if (voltage < UNDER_VOLTAGE_THRESHOLD) {
        *fault_code = FAULT_UNDER_VOLTAGE;
        return;
    }
    
    *fault_code = FAULT_NORMAL;
}

const char* fault_code_to_string(int fault_code) {
    switch (fault_code) {
        case FAULT_NORMAL: return "正常";
        case FAULT_OVER_VOLTAGE: return "过压故障";
        case FAULT_OVER_CURRENT: return "过流故障";
        case FAULT_OVER_TEMP: return "过热故障";
        case FAULT_UNDER_VOLTAGE: return "欠压故障";
        default: return "未知故障";
    }
}

int main() {
    printf("逆变器故障保护系统测试\n");
    printf("=====================\n\n");
    
    int fault_code;
    
    printf("测试用例1: 正常情况\n");
    printf("电压: 350.0V, 电流: 10.0A, 温度: 65.0°C\n");
    inverter_protection_check(350.0f, 10.0f, 65.0f, &fault_code);
    printf("故障代码: %d (%s)\n\n", fault_code, fault_code_to_string(fault_code));
    
    printf("测试用例2: 过压情况\n");
    printf("电压: 480.0V, 电流: 15.0A, 温度: 70.0°C\n");
    inverter_protection_check(480.0f, 15.0f, 70.0f, &fault_code);
    printf("故障代码: %d (%s)\n\n", fault_code, fault_code_to_string(fault_code));
    
    printf("测试用例3: 同时发生过压和过热（验证优先级）\n");
    printf("电压: 460.0V, 电流: 18.0A, 温度: 90.0°C\n");
    inverter_protection_check(460.0f, 18.0f, 90.0f, &fault_code);
    printf("故障代码: %d (%s)\n\n", fault_code, fault_code_to_string(fault_code));
    
    printf("测试用例4: 欠压情况\n");
    printf("电压: 280.0V, 电流: 12.0A, 温度: 60.0°C\n");
    inverter_protection_check(280.0f, 12.0f, 60.0f, &fault_code);
    printf("故障代码: %d (%s)\n\n", fault_code, fault_code_to_string(fault_code));
    
    printf("测试用例5: 过流情况\n");
    printf("电压: 350.0V, 电流: 25.0A, 温度: 70.0°C\n");
    inverter_protection_check(350.0f, 25.0f, 70.0f, &fault_code);
    printf("故障代码: %d (%s)\n\n", fault_code, fault_code_to_string(fault_code));
    
    printf("测试用例6: 过热情况\n");
    printf("电压: 350.0V, 电流: 15.0A, 温度: 88.0°C\n");
    inverter_protection_check(350.0f, 15.0f, 88.0f, &fault_code);
    printf("故障代码: %d (%s)\n\n", fault_code, fault_code_to_string(fault_code));
    
    return 0;
}