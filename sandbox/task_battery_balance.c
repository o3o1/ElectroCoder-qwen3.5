#include <stdio.h>
#include <stdlib.h>

#define VOLTAGE_TO_ENERGY_RATIO 100.0f
#define ENERGY_TO_VOLTAGE_RATIO 0.0001f
#define BALANCE_THRESHOLD 0.05f
#define TRANSFER_RATIO 0.05f

typedef struct {
    float voltage;
    float energy;
    int balanced;
} BatteryCell;

void balance_energy(BatteryCell cells[], int n) {
    if (n <= 0) return;
    
    int max_idx = 0;
    int min_idx = 0;
    
    for (int i = 1; i < n; i++) {
        if (cells[i].voltage > cells[max_idx].voltage) {
            max_idx = i;
        }
        if (cells[i].voltage < cells[min_idx].voltage) {
            min_idx = i;
        }
    }
    
    float voltage_diff = cells[max_idx].voltage - cells[min_idx].voltage;
    
    if (voltage_diff > BALANCE_THRESHOLD) {
        float transfer_energy = cells[max_idx].energy * TRANSFER_RATIO;
        
        cells[max_idx].energy -= transfer_energy;
        cells[min_idx].energy += transfer_energy;
        
        cells[max_idx].voltage = cells[max_idx].energy * ENERGY_TO_VOLTAGE_RATIO;
        cells[min_idx].voltage = cells[min_idx].energy * ENERGY_TO_VOLTAGE_RATIO;
        
        cells[max_idx].balanced = 1;
        cells[min_idx].balanced = 1;
    }
}

void print_cells(BatteryCell cells[], int n, const char* title) {
    printf("%s\n", title);
    for (int i = 0; i < n; i++) {
        printf("Cell%d: %.2fV, %.2fmJ", i+1, cells[i].voltage, cells[i].energy);
        if (cells[i].balanced) {
            printf(" (均衡)");
        }
        printf("\n");
    }
    printf("\n");
}

void reset_balanced_state(BatteryCell cells[], int n) {
    for (int i = 0; i < n; i++) {
        cells[i].balanced = 0;
    }
}

void test_case1() {
    printf("=== 测试用例1 ===\n");
    BatteryCell cells[4] = {
        {3.65f, 3.65f * VOLTAGE_TO_ENERGY_RATIO, 0},
        {3.70f, 3.70f * VOLTAGE_TO_ENERGY_RATIO, 0},
        {3.60f, 3.60f * VOLTAGE_TO_ENERGY_RATIO, 0},
        {3.75f, 3.75f * VOLTAGE_TO_ENERGY_RATIO, 0}
    };
    
    print_cells(cells, 4, "初始状态：");
    balance_energy(cells, 4);
    print_cells(cells, 4, "均衡后：");
}

void test_case2() {
    printf("=== 测试用例2 ===\n");
    BatteryCell cells[4] = {
        {3.80f, 3.80f * VOLTAGE_TO_ENERGY_RATIO, 0},
        {3.82f, 3.82f * VOLTAGE_TO_ENERGY_RATIO, 0},
        {3.78f, 3.78f * VOLTAGE_TO_ENERGY_RATIO, 0},
        {3.85f, 3.85f * VOLTAGE_TO_ENERGY_RATIO, 0}
    };
    
    print_cells(cells, 4, "初始状态：");
    balance_energy(cells, 4);
    print_cells(cells, 4, "均衡后：");
}

void test_case3() {
    printf("=== 测试用例3 ===\n");
    BatteryCell cells[4] = {
        {3.90f, 3.90f * VOLTAGE_TO_ENERGY_RATIO, 0},
        {3.91f, 3.91f * VOLTAGE_TO_ENERGY_RATIO, 0},
        {3.89f, 3.89f * VOLTAGE_TO_ENERGY_RATIO, 0},
        {3.92f, 3.92f * VOLTAGE_TO_ENERGY_RATIO, 0}
    };
    
    print_cells(cells, 4, "初始状态：");
    balance_energy(cells, 4);
    print_cells(cells, 4, "均衡后：");
}

int main() {
    test_case1();
    test_case2();
    test_case3();
    return 0;
}