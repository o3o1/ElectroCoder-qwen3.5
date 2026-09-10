#include <stdio.h>
#include <math.h>

#define ADC_VOLTAGE 0.0f
#define ADC_CURRENT 0.0f
#define SWITCH_STATE 0
#define ZVS_THRESHOLD 0.5f
#define ZCS_THRESHOLD 0.1f

int should_switch_on(float voltage, float current) {
    if (fabs(voltage) < ZVS_THRESHOLD && fabs(current) < ZCS_THRESHOLD) {
        return 1;
    }
    return 0;
}

int main() {
    float test_cases[][2] = {
        {0.2f, 0.05f},
        {1.0f, 0.05f},
        {0.2f, 0.5f},
        {-0.3f, 0.08f},
        {0.4f, -0.09f}
    };
    
    int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);
    
    for (int i = 0; i < num_cases; i++) {
        float voltage = test_cases[i][0];
        float current = test_cases[i][1];
        int result = should_switch_on(voltage, current);
        
        if (result) {
            printf("voltage=%.2fV, current=%.2fA -> can switch on\n", voltage, current);
        } else {
            printf("voltage=%.2fV, current=%.2fA -> cannot switch on\n", voltage, current);
        }
    }
    
    return 0;
}