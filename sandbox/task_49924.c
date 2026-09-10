#include <stdio.h>
#include <math.h>

#define BASE_DEADTIME 0.5f
#define MAX_DEADTIME 3.0f
#define CURRENT_COMP_MAX 1.5f
#define VOLTAGE_COMP_MAX 0.3f
#define CURRENT_COMP_RATE 0.02f
#define VOLTAGE_COMP_RATE 0.001f
#define TEMP_THRESHOLD 85.0f
#define TEMP_COMP_RATE 0.005f

float adaptive_deadtime_calc(float load_current, float dc_voltage, float temperature) {
    float deadtime = BASE_DEADTIME;
    
    float current_comp = load_current * CURRENT_COMP_RATE;
    if (current_comp > CURRENT_COMP_MAX) {
        current_comp = CURRENT_COMP_MAX;
    }
    deadtime += current_comp;
    
    float voltage_comp = dc_voltage * VOLTAGE_COMP_RATE;
    if (voltage_comp > VOLTAGE_COMP_MAX) {
        voltage_comp = VOLTAGE_COMP_MAX;
    }
    deadtime += voltage_comp;
    
    if (temperature > TEMP_THRESHOLD) {
        float temp_comp = (temperature - TEMP_THRESHOLD) * TEMP_COMP_RATE;
        deadtime += temp_comp;
    }
    
    if (deadtime > MAX_DEADTIME) {
        deadtime = MAX_DEADTIME;
    }
    
    return deadtime;
}

int main() {
    printf("DC/DC变换器死区时间自适应调整算法测试\n");
    printf("========================================\n");
    
    float test_cases[4][3] = {
        {2.5f, 48.0f, 25.0f},
        {15.0f, 96.0f, 30.0f},
        {8.0f, 72.0f, 95.0f},
        {25.0f, 120.0f, 105.0f}
    };
    
    const char* case_names[4] = {"轻载情况", "重载情况", "高温情况", "极限情况"};
    
    for (int i = 0; i < 4; i++) {
        float load_current = test_cases[i][0];
        float dc_voltage = test_cases[i][1];
        float temperature = test_cases[i][2];
        
        float deadtime = adaptive_deadtime_calc(load_current, dc_voltage, temperature);
        
        printf("测试用例%d(%s): 负载电流=%.1fA, 直流电压=%.1fV, 温度=%.1f°C => 死区时间=%.3fμs\n",
               i+1, case_names[i], load_current, dc_voltage, temperature, deadtime);
    }
    
    printf("\n算法参数说明:\n");
    printf("- 基础死区时间: %.1fμs\n", BASE_DEADTIME);
    printf("- 电流补偿: 每1A增加%.3fμs (上限%.1fμs)\n", CURRENT_COMP_RATE, CURRENT_COMP_MAX);
    printf("- 电压补偿: 每10V增加%.3fμs (上限%.1fμs)\n", VOLTAGE_COMP_RATE*10, VOLTAGE_COMP_MAX);
    printf("- 温度补偿: >%.1f°C时每1°C增加%.3fμs\n", TEMP_THRESHOLD, TEMP_COMP_RATE);
    printf("- 总死区时间上限: %.1fμs\n", MAX_DEADTIME);
    
    return 0;
}