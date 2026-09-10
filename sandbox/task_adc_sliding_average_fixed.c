#include <stdio.h>
#include <stdlib.h>

#define WINDOW_SIZE 10

typedef struct {
    int buffer[WINDOW_SIZE];
    int index;
    int count;
    int sum;
} SlidingWindow;

void init_window(SlidingWindow *window) {
    for (int i = 0; i < WINDOW_SIZE; i++) {
        window->buffer[i] = 0;
    }
    window->index = 0;
    window->count = 0;
    window->sum = 0;
}

int sliding_average_filter(SlidingWindow *window, int new_sample) {
    if (window->count == WINDOW_SIZE) {
        window->sum -= window->buffer[window->index];
    } else {
        window->count++;
    }
    
    window->buffer[window->index] = new_sample;
    window->sum += new_sample;
    
    window->index = (window->index + 1) % WINDOW_SIZE;
    
    return window->sum / window->count;
}

int main() {
    SlidingWindow window;
    init_window(&window);
    
    int adc_samples[] = {100, 105, 98, 102, 110, 95, 103, 107, 99, 101};
    int sample_count = sizeof(adc_samples) / sizeof(adc_samples[0]);
    
    printf("Initial 10 samples filtering process:\n");
    for (int i = 0; i < sample_count; i++) {
        int filtered_value = sliding_average_filter(&window, adc_samples[i]);
        printf("Sample: %d, Filtered average: %d\n", adc_samples[i], filtered_value);
    }
    
    int new_samples[] = {104, 96, 108, 97, 105};
    int new_sample_count = sizeof(new_samples) / sizeof(new_samples[0]);
    
    printf("\nAdditional 5 new samples filtering process:\n");
    for (int i = 0; i < new_sample_count; i++) {
        int filtered_value = sliding_average_filter(&window, new_samples[i]);
        printf("Sample: %d, Filtered average: %d\n", new_samples[i], filtered_value);
    }
    
    return 0;
}