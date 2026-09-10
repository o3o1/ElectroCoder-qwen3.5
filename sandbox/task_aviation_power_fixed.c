#include <stdio.h>

int detect_voltage_mode(float input_voltage) {
    if (input_voltage >= 20.0f && input_voltage <= 36.0f) {
        return 1;
    } else if (input_voltage >= 90.0f && input_voltage <= 130.0f) {
        return 2;
    } else {
        return 0;
    }
}

int main() {
    float test_voltages[] = {25.0f, 120.0f, 50.0f};
    int num_tests = sizeof(test_voltages) / sizeof(test_voltages[0]);
    
    for (int i = 0; i < num_tests; i++) {
        float voltage = test_voltages[i];
        int mode = detect_voltage_mode(voltage);
        
        printf("测试电压: %.2fV -> 模式: ", voltage);
        if (mode == 1) {
            printf("28V模式\n");
        } else if (mode == 2) {
            printf("115V模式\n");
        } else {
            printf("电压异常\n");
        }
    }
    
    return 0;
}