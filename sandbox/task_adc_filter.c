#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define WINDOW_SIZE 8

typedef struct {
    uint16_t buffer[WINDOW_SIZE];
    uint8_t index;
    uint32_t sum;
    bool is_full;
} SlidingWindow;

void filter_init(SlidingWindow* window) {
    for (int i = 0; i < WINDOW_SIZE; i++) {
        window->buffer[i] = 0;
    }
    window->index = 0;
    window->sum = 0;
    window->is_full = false;
}

uint16_t filter_update(SlidingWindow* window, uint16_t new_sample) {
    if (window->is_full) {
        window->sum -= window->buffer[window->index];
    }
    
    window->buffer[window->index] = new_sample;
    window->sum += new_sample;
    
    window->index++;
    if (window->index >= WINDOW_SIZE) {
        window->index = 0;
        window->is_full = true;
    }
    
    uint8_t count = window->is_full ? WINDOW_SIZE : window->index;
    uint32_t avg = (window->sum + count/2) / count;
    return (uint16_t)avg;
}

int main() {
    SlidingWindow window;
    filter_init(&window);
    
    uint16_t samples[] = {100, 102, 98, 105, 103, 101, 99, 104, 102, 100};
    int sample_count = sizeof(samples) / sizeof(samples[0]);
    
    for (int i = 0; i < sample_count; i++) {
        uint16_t filtered = filter_update(&window, samples[i]);
        printf("采样值：[%d] 滤波后：[%d]\n", samples[i], filtered);
    }
    
    printf("\n最终窗口元素：");
    for (int i = 0; i < WINDOW_SIZE; i++) {
        printf("%d ", window.buffer[i]);
    }
    printf("\n");
    
    return 0;
}