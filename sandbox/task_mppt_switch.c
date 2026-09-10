#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
    int id;
    float (*track)(float V, float I);
    char name[20];
} MPPT_Algorithm;

float po_track(float V, float I) {
    return V * I * 0.85f;
}

float inc_track(float V, float I) {
    return V * I * 0.92f;
}

float cv_track(float V, float I) {
    return V * I * 0.78f;
}

int select_algorithm(float delta_P, float delta_V, int current_id) {
    float abs_delta_P = fabsf(delta_P);
    float abs_delta_V = fabsf(delta_V);
    
    if (abs_delta_P > 10.0f && abs_delta_V > 2.0f) {
        return 2;
    }
    
    if (abs_delta_P < 1.0f && abs_delta_V < 0.1f) {
        return 1;
    }
    
    return current_id;
}

int main() {
    MPPT_Algorithm algorithms[3];
    
    algorithms[0].id = 1;
    algorithms[0].track = po_track;
    snprintf(algorithms[0].name, 20, "P&O");
    
    algorithms[1].id = 2;
    algorithms[1].track = inc_track;
    snprintf(algorithms[1].name, 20, "INC");
    
    algorithms[2].id = 3;
    algorithms[2].track = cv_track;
    snprintf(algorithms[2].name, 20, "CV");
    
    float test_data[][4] = {
        {30.0f, 5.0f, 15.0f, 3.0f},
        {31.0f, 4.8f, 0.5f, 0.05f},
        {30.5f, 5.2f, 8.0f, 1.5f},
        {29.8f, 5.1f, 12.0f, 2.5f},
        {30.2f, 5.0f, 0.3f, 0.08f}
    };
    
    int current_algorithm_id = 1;
    
    printf("MPPT算法切换演示:\n");
    printf("==================\n");
    
    for (int i = 0; i < 5; i++) {
        float V = test_data[i][0];
        float I = test_data[i][1];
        float delta_P = test_data[i][2];
        float delta_V = test_data[i][3];
        
        current_algorithm_id = select_algorithm(delta_P, delta_V, current_algorithm_id);
        
        MPPT_Algorithm* selected_algo = NULL;
        for (int j = 0; j < 3; j++) {
            if (algorithms[j].id == current_algorithm_id) {
                selected_algo = &algorithms[j];
                break;
            }
        }
        
        if (selected_algo) {
            float power = selected_algo->track(V, I);
            printf("时间点%d: V=%.1fV, I=%.1fA, delta_P=%.1fW, delta_V=%.2fV\n", 
                   i+1, V, I, delta_P, delta_V);
            printf("  选中算法: %s, 计算功率: %.2fW\n\n", 
                   selected_algo->name, power);
        }
    }
    
    return 0;
}