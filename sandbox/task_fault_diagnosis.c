#include <stdio.h>

// 故障类型枚举
typedef enum {
    FAULT_NONE = 0,
    FAULT_LIGHT,
    FAULT_HEAVY
} FaultType;

// 传感器正常范围阈值
#define VOLTAGE_MIN 10.0f
#define VOLTAGE_MAX 15.0f
#define VOLTAGE_CRITICAL_MIN 8.0f
#define VOLTAGE_CRITICAL_MAX 18.0f

#define CURRENT_MIN 0.0f
#define CURRENT_MAX 10.0f
#define CURRENT_CRITICAL_MAX 15.0f

#define TEMPERATURE_MIN 0.0f
#define TEMPERATURE_MAX 60.0f
#define TEMPERATURE_CRITICAL_MAX 80.0f

// 故障诊断函数
FaultType diagnose_fault(float voltage, float current, float temperature) {
    int heavy_fault = 0;
    int light_fault = 0;
    
    // 检查电压
    if (voltage < VOLTAGE_CRITICAL_MIN || voltage > VOLTAGE_CRITICAL_MAX) {
        heavy_fault = 1;
    } else if (voltage < VOLTAGE_MIN || voltage > VOLTAGE_MAX) {
        light_fault = 1;
    }
    
    // 检查电流
    if (current > CURRENT_CRITICAL_MAX) {
        heavy_fault = 1;
    } else if (current > CURRENT_MAX) {
        light_fault = 1;
    }
    
    // 检查温度
    if (temperature > TEMPERATURE_CRITICAL_MAX) {
        heavy_fault = 1;
    } else if (temperature > TEMPERATURE_MAX) {
        light_fault = 1;
    }
    
    // 按重故障优先原则返回
    if (heavy_fault) {
        return FAULT_HEAVY;
    } else if (light_fault) {
        return FAULT_LIGHT;
    } else {
        return FAULT_NONE;
    }
}

// 故障类型转字符串
const char* fault_type_to_string(FaultType type) {
    switch (type) {
        case FAULT_NONE: return "正常";
        case FAULT_LIGHT: return "轻故障";
        case FAULT_HEAVY: return "重故障";
        default: return "未知";
    }
}

int main() {
    // 5组测试用例
    float test_cases[5][3] = {
        // 电压, 电流, 温度
        {12.5f, 5.0f, 45.0f},    // 正常情况
        {9.5f, 6.0f, 55.0f},     // 电压轻故障
        {7.5f, 12.0f, 85.0f},    // 多重重故障
        {13.5f, 11.0f, 65.0f},   // 电流轻故障+温度轻故障
        {16.5f, 8.0f, 50.0f}     // 电压轻故障
    };
    
    printf("电力电子变换器故障自诊断系统\n");
    printf("==============================\n");
    
    for (int i = 0; i < 5; i++) {
        float voltage = test_cases[i][0];
        float current = test_cases[i][1];
        float temperature = test_cases[i][2];
        
        FaultType result = diagnose_fault(voltage, current, temperature);
        
        printf("电压=%.1fV, 电流=%.1fA, 温度=%.1f°C -> 诊断结果：%s\n",
               voltage, current, temperature, fault_type_to_string(result));
    }
    
    return 0;
}