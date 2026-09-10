#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// 滑动平均滤波器结构体
typedef struct {
    int *buffer;        // 缓冲区数组
    int size;           // 缓冲区大小N
    int index;          // 当前缓冲区索引
    int sum;            // 当前累加和
    bool is_full;       // 缓冲区是否已满的标志
} SlidingAverageFilter;

// 初始化滤波器
void filter_init(SlidingAverageFilter *filter, int size) {
    filter->buffer = (int *)malloc(size * sizeof(int));
    filter->size = size;
    filter->index = 0;
    filter->sum = 0;
    filter->is_full = false;
    
    // 清空缓冲区
    for (int i = 0; i < size; i++) {
        filter->buffer[i] = 0;
    }
}

// 更新滤波器，输入新值并返回滤波后的平均值
int filter_update(SlidingAverageFilter *filter, int new_value) {
    // 减去即将被覆盖的旧值
    if (filter->is_full) {
        filter->sum -= filter->buffer[filter->index];
    }
    
    // 添加新值
    filter->sum += new_value;
    filter->buffer[filter->index] = new_value;
    
    // 更新索引
    filter->index = (filter->index + 1) % filter->size;
    
    // 检查缓冲区是否已满
    if (!filter->is_full && filter->index == 0) {
        filter->is_full = true;
    }
    
    // 计算平均值
    int count = filter->is_full ? filter->size : filter->index;
    return filter->sum / count;
}

// 获取当前平均值
int filter_get_average(SlidingAverageFilter *filter) {
    int count = filter->is_full ? filter->size : filter->index;
    if (count == 0) return 0;
    return filter->sum / count;
}

// 打印缓冲区内容
void filter_print_buffer(SlidingAverageFilter *filter) {
    printf("缓冲区数据: ");
    for (int i = 0; i < filter->size; i++) {
        printf("%d ", filter->buffer[i]);
    }
    printf("\n");
}

int main() {
    // 创建大小为5的滑动平均滤波器
    SlidingAverageFilter filter;
    filter_init(&filter, 5);
    
    // 模拟ADC采样数据序列
    int adc_samples[] = {10, 20, 30, 40, 50, 60, 70, 80};
    int sample_count = sizeof(adc_samples) / sizeof(adc_samples[0]);
    
    printf("滑动平均滤波器测试 (缓冲区大小: %d)\n", filter.size);
    printf("========================================\n");
    
    // 依次输入每个采样值并打印滤波结果
    for (int i = 0; i < sample_count; i++) {
        int filtered_value = filter_update(&filter, adc_samples[i]);
        printf("输入: %d, 滤波后: %d\n", adc_samples[i], filtered_value);
    }
    
    printf("\n");
    printf("当前平均值: %d\n", filter_get_average(&filter));
    filter_print_buffer(&filter);
    
    // 释放内存
    free(filter.buffer);
    
    return 0;
}