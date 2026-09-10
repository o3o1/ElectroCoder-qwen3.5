#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

typedef struct {
    int id;
    float voltage_out;
    float voltage_ref;
    float duty_cycle;
    float kp, ki;
    float integral_error;
} DCDCModule;

void balance_control(DCDCModule *modules, int n, float dt) {
    if (n <= 0) return;
    
    float sum_voltage = 0.0f;
    for (int i = 0; i < n; i++) {
        sum_voltage += modules[i].voltage_out;
    }
    float avg_voltage = sum_voltage / n;
    
    for (int i = 0; i < n; i++) {
        float error = modules[i].voltage_out - avg_voltage;
        
        modules[i].integral_error += error * dt;
        
        float control = modules[i].kp * error + modules[i].ki * modules[i].integral_error;
        modules[i].duty_cycle += control;
        
        if (modules[i].duty_cycle < 0.05f) modules[i].duty_cycle = 0.05f;
        if (modules[i].duty_cycle > 0.95f) modules[i].duty_cycle = 0.95f;
    }
}

int main() {
    srand(time(NULL));
    
    DCDCModule modules[3];
    
    modules[0] = (DCDCModule){1, 95.0f, 100.0f, 0.8f, 0.01f, 0.1f, 0.0f};
    modules[1] = (DCDCModule){2, 105.0f, 100.0f, 0.85f, 0.012f, 0.12f, 0.0f};
    modules[2] = (DCDCModule){3, 100.0f, 100.0f, 0.82f, 0.011f, 0.11f, 0.0f};
    
    float dt = 0.01f;
    int cycles = 5;
    
    printf("高压DC/DC变换器串联均压控制仿真\n");
    printf("初始状态:\n");
    for (int i = 0; i < 3; i++) {
        printf("模块%d: 电压=%.2fV, 占空比=%.3f\n", 
               modules[i].id, modules[i].voltage_out, modules[i].duty_cycle);
    }
    printf("\n");
    
    for (int cycle = 0; cycle < cycles; cycle++) {
        printf("周期 %d:\n", cycle + 1);
        
        for (int i = 0; i < 3; i++) {
            printf("  模块%d: 电压=%.2fV, 占空比=%.3f", 
                   modules[i].id, modules[i].voltage_out, modules[i].duty_cycle);
            
            float avg = (modules[0].voltage_out + modules[1].voltage_out + modules[2].voltage_out) / 3.0f;
            printf(", 与平均差=%.2fV\n", modules[i].voltage_out - avg);
        }
        
        balance_control(modules, 3, dt);
        
        for (int i = 0; i < 3; i++) {
            modules[i].voltage_out = modules[i].duty_cycle * 120.0f + (rand() % 10 - 5) * 0.1f;
        }
        
        printf("\n");
    }
    
    printf("最终状态:\n");
    for (int i = 0; i < 3; i++) {
        printf("模块%d: 电压=%.2fV, 占空比=%.3f\n", 
               modules[i].id, modules[i].voltage_out, modules[i].duty_cycle);
    }
    
    return 0;
}