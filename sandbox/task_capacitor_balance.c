#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int id;
    float voltage;
    float threshold;
    int alarm_flag;
} Capacitor;

void check_voltage_balance(Capacitor caps[], int n, float tolerance) {
    if (n <= 0) return;
    
    float sum = 0.0;
    for (int i = 0; i < n; i++) {
        sum += caps[i].voltage;
    }
    float avg_voltage = sum / n;
    
    printf("平均电压: %.1fV\n", avg_voltage);
    
    for (int i = 0; i < n; i++) {
        float deviation = ((caps[i].voltage - avg_voltage) / avg_voltage) * 100.0;
        if (deviation > tolerance || deviation < -tolerance) {
            printf("均压检查: 电容%d电压偏差过大(%.1f%%)\n", caps[i].id, deviation);
        }
    }
}

void detect_overvoltage(Capacitor caps[], int n) {
    for (int i = 0; i < n; i++) {
        if (caps[i].voltage > caps[i].threshold) {
            caps[i].alarm_flag = 1;
            printf("电容%d: 电压=%.1fV, 状态=过压告警\n", caps[i].id, caps[i].voltage);
        }
    }
}

void protection_action(Capacitor caps[], int n) {
    for (int i = 0; i < n; i++) {
        if (caps[i].alarm_flag == 1) {
            caps[i].voltage = caps[i].threshold * 0.95;
            caps[i].alarm_flag = 0;
        }
    }
    printf("保护动作执行完成\n");
}

int main() {
    Capacitor caps[5];
    
    caps[0] = (Capacitor){1, 105.0, 110.0, 0};
    caps[1] = (Capacitor){2, 112.5, 110.0, 0};
    caps[2] = (Capacitor){3, 108.0, 115.0, 0};
    caps[3] = (Capacitor){4, 102.0, 110.0, 0};
    caps[4] = (Capacitor){5, 107.5, 110.0, 0};
    
    printf("初始状态:\n");
    for (int i = 0; i < 5; i++) {
        printf("电容%d: 电压=%.1fV, 阈值=%.1fV\n", caps[i].id, caps[i].voltage, caps[i].threshold);
    }
    
    printf("\n均压检查:\n");
    check_voltage_balance(caps, 5, 5.0);
    
    printf("\n过压检测:\n");
    detect_overvoltage(caps, 5);
    
    printf("\n保护动作:\n");
    protection_action(caps, 5);
    
    printf("\n最终状态:\n");
    for (int i = 0; i < 5; i++) {
        const char* status = caps[i].alarm_flag == 1 ? "过压告警" : "正常";
        printf("电容%d: 电压=%.1fV, 状态=%s\n", caps[i].id, caps[i].voltage, status);
    }
    
    return 0;
}