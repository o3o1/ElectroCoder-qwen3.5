#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    float* buffer;
    int window_size;
    int current_index;
    float current_sum;
    int is_full;
} MovingAverageFilter;

void filter_init(MovingAverageFilter* filter, int window_size) {
    filter->buffer = (float*)malloc(window_size * sizeof(float));
    filter->window_size = window_size;
    filter->current_index = 0;
    filter->current_sum = 0.0f;
    filter->is_full = 0;
    memset(filter->buffer, 0, window_size * sizeof(float));
}

float filter_update(MovingAverageFilter* filter, float new_sample) {
    if (filter->is_full) {
        filter->current_sum -= filter->buffer[filter->current_index];
    }
    
    filter->buffer[filter->current_index] = new_sample;
    filter->current_sum += new_sample;
    
    filter->current_index++;
    if (filter->current_index >= filter->window_size) {
        filter->current_index = 0;
        filter->is_full = 1;
    }
    
    int valid_samples = filter->is_full ? filter->window_size : filter->current_index;
    return filter->current_sum / valid_samples;
}

void filter_reset(MovingAverageFilter* filter) {
    filter->current_index = 0;
    filter->current_sum = 0.0f;
    filter->is_full = 0;
    memset(filter->buffer, 0, filter->window_size * sizeof(float));
}

int main() {
    MovingAverageFilter filter;
    
    printf("初始化滤波器...\n");
    filter_init(&filter, 5);
    
    float samples[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
    int num_samples = sizeof(samples) / sizeof(samples[0]);
    
    for (int i = 0; i < num_samples; i++) {
        float avg = filter_update(&filter, samples[i]);
        printf("输入%f，平均值：%f\n", samples[i], avg);
    }
    
    printf("\n重置滤波器...\n");
    filter_reset(&filter);
    
    float new_samples[] = {10.0, 20.0};
    int num_new_samples = sizeof(new_samples) / sizeof(new_samples[0]);
    
    for (int i = 0; i < num_new_samples; i++) {
        float avg = filter_update(&filter, new_samples[i]);
        printf("重置后输入%f，平均值：%f\n", new_samples[i], avg);
    }
    
    free(filter.buffer);
    return 0;
}