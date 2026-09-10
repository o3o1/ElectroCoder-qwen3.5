#include <stdio.h>
#include <math.h>

#define V_NOMINAL 12.0f
#define D_NOMINAL 0.5f

float pwm_compensate(float v_actual, float v_nominal, float d_nominal) {
    if (v_actual <= 0.0f) {
        return 0.0f;
    }
    
    float d_compensated = d_nominal * (v_nominal / v_actual);
    
    if (d_compensated > 1.0f) {
        return 1.0f;
    } else if (d_compensated < 0.0f) {
        return 0.0f;
    }
    
    return d_compensated;
}

int main() {
    float test_cases[][2] = {
        {12.0f, 0.5f},
        {18.0f, 0.5f},
        {9.0f, 0.5f},
        {0.0f, 0.5f}
    };
    
    const char* status_names[] = {
        "正常",
        "过压",
        "欠压",
        "无效"
    };
    
    printf("PWM输入电压补偿算法测试\n");
    printf("额定电压: %.3fV, 额定占空比: %.3f\n\n", V_NOMINAL, D_NOMINAL);
    
    for (int i = 0; i < 4; i++) {
        float v_actual = test_cases[i][0];
        float d_nominal = test_cases[i][1];
        float d_compensated = pwm_compensate(v_actual, V_NOMINAL, d_nominal);
        
        int status = 0;
        if (v_actual <= 0.0f) {
            status = 3;
        } else if (v_actual > V_NOMINAL * 1.1f) {
            status = 1;
        } else if (v_actual < V_NOMINAL * 0.9f) {
            status = 2;
        } else {
            status = 0;
        }
        
        printf("测试用例 %d:\n", i + 1);
        printf("  实际电压: %.3fV\n", v_actual);
        printf("  补偿后占空比: %.3f\n", d_compensated);
        printf("  补偿状态: %s\n\n", status_names[status]);
    }
    
    return 0;
}