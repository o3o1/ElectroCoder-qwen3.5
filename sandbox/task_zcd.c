#include <stdio.h>
#include <stdlib.h>

// 过零检测函数
int zero_crossing_detection(int signal_value, int *debounce_counter, int threshold) {
    static int last_value = 0;
    static int initialized = 0;
    
    // 初始化last_value
    if (!initialized) {
        last_value = signal_value;
        initialized = 1;
        *debounce_counter = 0;
        return 0;
    }
    
    // 判断是否跨越零点
    if ((last_value >= 0 && signal_value < 0) || (last_value <= 0 && signal_value > 0)) {
        // 跨越零点，计数器加1
        (*debounce_counter)++;
        
        // 检查是否达到阈值
        if (*debounce_counter >= threshold) {
            // 确认过零事件，重置计数器
            *debounce_counter = 0;
            last_value = signal_value;
            return 1;
        }
    } else {
        // 未跨越零点，重置计数器
        *debounce_counter = 0;
    }
    
    // 更新历史值
    last_value = signal_value;
    return 0;
}

int main() {
    // 模拟采样数据
    int samples[] = {50, 20, -10, -30, 5, -5, -8, 15, 25, -15, -25};
    int num_samples = sizeof(samples) / sizeof(samples[0]);
    int threshold = 2;
    int debounce_counter = 0;
    int zero_cross_count = 0;
    
    printf("过零检测模拟\n");
    printf("采样数据: ");
    for (int i = 0; i < num_samples; i++) {
        printf("%d ", samples[i]);
    }
    printf("\n防抖阈值: %d\n\n", threshold);
    
    printf("采样序号\t采样值\t检测结果\n");
    printf("--------------------------------\n");
    
    // 对每个采样值进行检测
    for (int i = 0; i < num_samples; i++) {
        int result = zero_crossing_detection(samples[i], &debounce_counter, threshold);
        
        if (result) {
            zero_cross_count++;
            printf("%d\t\t%d\t检测到过零\n", i, samples[i]);
        } else {
            printf("%d\t\t%d\t未检测到\n", i, samples[i]);
        }
    }
    
    printf("\n--------------------------------\n");
    printf("总共检测到过零次数: %d\n", zero_cross_count);
    
    return 0;
}