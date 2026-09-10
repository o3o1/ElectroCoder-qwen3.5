#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int window_size;
    int *buffer;
    int buffer_index;
    int sum;
    int count;
} MovingAverageFilter;

void filter_init(MovingAverageFilter *filter, int window_size) {
    filter->window_size = window_size;
    filter->buffer = (int *)malloc(window_size * sizeof(int));
    filter->buffer_index = 0;
    filter->sum = 0;
    filter->count = 0;
}

int filter_update(MovingAverageFilter *filter, int new_sample) {
    if (filter->count < filter->window_size) {
        filter->sum += new_sample;
        filter->buffer[filter->buffer_index] = new_sample;
        filter->buffer_index = (filter->buffer_index + 1) % filter->window_size;
        filter->count++;
    } else {
        int oldest = filter->buffer[filter->buffer_index];
        filter->sum = filter->sum - oldest + new_sample;
        filter->buffer[filter->buffer_index] = new_sample;
        filter->buffer_index = (filter->buffer_index + 1) % filter->window_size;
    }
    
    return filter->sum / filter->count;
}

void filter_cleanup(MovingAverageFilter *filter) {
    free(filter->buffer);
    filter->buffer = NULL;
}

int main() {
    MovingAverageFilter filter;
    filter_init(&filter, 5);
    
    int adc_samples[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    int sample_count = sizeof(adc_samples) / sizeof(adc_samples[0]);
    
    for (int i = 0; i < sample_count; i++) {
        int filtered = filter_update(&filter, adc_samples[i]);
        printf("原始值: %d, 滤波后: %d\n", adc_samples[i], filtered);
    }
    
    filter_cleanup(&filter);
    return 0;
}