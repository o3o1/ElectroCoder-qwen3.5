#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define VOLTAGE_MIN 200.0f
#define VOLTAGE_MAX 250.0f
#define CURRENT_SURGE_THRESHOLD 1.5f
#define HISTORY_WINDOW 5

bool check_fuse_failure(float current, float voltage, float *current_history, int history_size) {
    static int history_index = 0;
    static int history_count = 0;
    
    current_history[history_index] = current;
    history_index = (history_index + 1) % history_size;
    if (history_count < history_size) {
        history_count++;
    }
    
    bool condition1 = false;
    bool condition2 = false;
    
    if (history_count >= HISTORY_WINDOW) {
        float sum = 0.0f;
        for (int i = 0; i < HISTORY_WINDOW; i++) {
            int idx = (history_index - i - 1 + history_size) % history_size;
            sum += current_history[idx];
        }
        float avg_current = sum / HISTORY_WINDOW;
        
        if (current > avg_current * CURRENT_SURGE_THRESHOLD) {
            condition1 = true;
        }
    }
    
    if (voltage < VOLTAGE_MIN || voltage > VOLTAGE_MAX) {
        condition2 = true;
    }
    
    return condition1 && condition2;
}

int main() {
    const int history_size = 10;
    float current_history[history_size];
    memset(current_history, 0, sizeof(current_history));
    
    printf("=== 熔断器熔断检测算法测试 ===\n\n");
    
    printf("测试用例1: 正常情况（电流电压均在正常范围）\n");
    for (int i = 0; i < 5; i++) {
        check_fuse_failure(10.0f, 220.0f, current_history, history_size);
    }
    bool result1 = check_fuse_failure(12.0f, 230.0f, current_history, history_size);
    printf("检测结果: %s\n\n", result1 ? "熔断" : "正常");
    
    printf("测试用例2: 仅电流突变但电压正常\n");
    memset(current_history, 0, sizeof(current_history));
    for (int i = 0; i < 5; i++) {
        check_fuse_failure(10.0f, 220.0f, current_history, history_size);
    }
    bool result2 = check_fuse_failure(20.0f, 230.0f, current_history, history_size);
    printf("检测结果: %s\n\n", result2 ? "熔断" : "正常");
    
    printf("测试用例3: 电流电压同时异常\n");
    memset(current_history, 0, sizeof(current_history));
    for (int i = 0; i < 5; i++) {
        check_fuse_failure(10.0f, 220.0f, current_history, history_size);
    }
    bool result3 = check_fuse_failure(20.0f, 180.0f, current_history, history_size);
    printf("检测结果: %s\n\n", result3 ? "熔断" : "正常");
    
    return 0;
}