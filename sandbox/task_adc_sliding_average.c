#include <stdio.h>
#include <stdlib.h>

// 滑动窗口大小
#define WINDOW_SIZE 10

// 滑动窗口结构体
typedef struct {
    int buffer[WINDOW_SIZE];  // 数据缓冲区
    int index;                // 当前写入位置
    int count;                // 当前有效数据个数
    int sum;                  // 当前窗口内数据总和
} SlidingWindow;

// 初始化滑动窗口
void init_window(SlidingWindow *window) {
    for (int i = 0; i < WINDOW_SIZE; i++) {
        window->buffer[i] = 0;  // 初始化为0
    }
    window->index = 0;
    window->count = 0;
    window->sum = 0;
}

// 滑动平均滤波函数
int sliding_average_filter(SlidingWindow *window, int new_sample) {
    // 如果窗口已满，先减去即将被替换的旧值
    if (window->count == WINDOW_SIZE) {
        window->sum -= window->buffer[window->index];
    } else {
        window->count++;
    }
    
    // 添加新值到窗口
    window->buffer[window->index] = new_sample;
    window->sum += new_sample;
    
    // 更新索引（循环缓冲区）
    window->index = (window->index + 1) % WINDOW_SIZE;
    
    // 计算平均值（整数除法）
    return window->sum / window->count;
}

int main() {
    SlidingWindow window;
    init_window(&window);
    
    // 模拟ADC采样值
    int adc_samples[] = {100, 105, 98, 102, 110, 95, 103, 107, 99, 101};
    int sample_count = sizeof(adc_samples) / sizeof(adc_samples[0]);
    
    printf("初始10个采样值的滤波过程：\n");
    for (int i = 0; i < sample_count; i++) {
        int filtered_value = sliding_average_filter(&window, adc_samples[i]);
        printf("采样值: %d, 滤波后平均值: %d\n", adc_samples[i], filtered_value);
    }
    
    // 额外处理5个新采样值
    int new_samples[] = {104, 96, 108, 97, 105};
    int new_sample_count = sizeof(new_samples) / sizeof(new_samples[0]);
    
    printf("\n额外5个新采样值的滤波过程：\n");
    for (int i = 0; i < new_sample_count; i++) {
        int filtered_value = sliding_average_filter(&window, new_samples[i]);
        printf("采样值: %d, 滤波后平均值: %d\n", new_samples[i], filtered_value);
    }
    
    return 0;
}