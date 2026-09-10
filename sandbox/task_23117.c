#include <stdio.h>

int thermal_protection_action(float current_temp) {
    if (current_temp < 80.0f) {
        return 0;
    } else if (current_temp < 90.0f) {
        return 1;
    } else if (current_temp < 100.0f) {
        return 2;
    } else {
        return 3;
    }
}

const char* get_action_description(int action) {
    switch (action) {
        case 0: return "normal";
        case 1: return "reduce load to 80%";
        case 2: return "reduce load to 50%";
        case 3: return "shutdown";
        default: return "unknown";
    }
}

int main() {
    float test_temps[] = {75.5f, 85.0f, 95.8f, 105.2f};
    int num_tests = sizeof(test_temps) / sizeof(test_temps[0]);
    
    for (int i = 0; i < num_tests; i++) {
        float temp = test_temps[i];
        int action = thermal_protection_action(temp);
        const char* description = get_action_description(action);
        printf("Temperature: %.1f°C -> Action: %s\n", temp, description);
    }
    
    return 0;
}