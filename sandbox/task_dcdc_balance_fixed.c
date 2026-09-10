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
    
    modules[0].id = 1;
    modules[0].voltage_out = 95.0f;
    modules[0].voltage_ref = 100.0f;
    modules[0].duty_cycle = 0.8f;
    modules[0].kp = 0.01f;
    modules[0].ki = 0.1f;
    modules[0].integral_error = 0.0f;
    
    modules[1].id = 2;
    modules[1].voltage_out = 105.0f;
    modules[1].voltage_ref = 100.0f;
    modules[1].duty_cycle = 0.85f;
    modules[1].kp = 0.012f;
    modules[1].ki = 0.12f;
    modules[1].integral_error = 0.0f;
    
    modules[2].id = 3;
    modules[2].voltage_out = 100.0f;
    modules[2].voltage_ref = 100.0f;
    modules[2].duty_cycle = 0.82f;
    modules[2].kp = 0.011f;
    modules[2].ki = 0.11f;
    modules[2].integral_error = 0.0f;
    
    float dt = 0.01f;
    int cycles = 5;
    
    printf("High Voltage DC/DC Converter Series Balancing Control Simulation\n");
    printf("Initial State:\n");
    for (int i = 0; i < 3; i++) {
        printf("Module %d: Voltage=%.2fV, Duty Cycle=%.3f\n", 
               modules[i].id, modules[i].voltage_out, modules[i].duty_cycle);
    }
    printf("\n");
    
    for (int cycle = 0; cycle < cycles; cycle++) {
        printf("Cycle %d:\n", cycle + 1);
        
        for (int i = 0; i < 3; i++) {
            printf("  Module %d: Voltage=%.2fV, Duty Cycle=%.3f", 
                   modules[i].id, modules[i].voltage_out, modules[i].duty_cycle);
            
            float avg = (modules[0].voltage_out + modules[1].voltage_out + modules[2].voltage_out) / 3.0f;
            printf(", Diff from Avg=%.2fV\n", modules[i].voltage_out - avg);
        }
        
        balance_control(modules, 3, dt);
        
        for (int i = 0; i < 3; i++) {
            modules[i].voltage_out = modules[i].duty_cycle * 120.0f + (rand() % 10 - 5) * 0.1f;
        }
        
        printf("\n");
    }
    
    printf("Final State:\n");
    for (int i = 0; i < 3; i++) {
        printf("Module %d: Voltage=%.2fV, Duty Cycle=%.3f\n", 
               modules[i].id, modules[i].voltage_out, modules[i].duty_cycle);
    }
    
    return 0;
}