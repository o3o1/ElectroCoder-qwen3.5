#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define ADC_MAX 4095

typedef struct {
    uint16_t *buffer;
    int size;
    int count;
    int next_index;
    uint32_t sum;
} Window;

void init_window(Window *w, int size) {
    w->buffer = (uint16_t *)malloc(size * sizeof(uint16_t));
    w->size = size;
    w->count = 0;
    w->next_index = 0;
    w->sum = 0;
}

void add_sample(Window *w, uint16_t sample) {
    if (w->count < w->size) {
        w->buffer[w->next_index] = sample;
        w->sum += sample;
        w->count++;
    } else {
        w->sum -= w->buffer[w->next_index];
        w->buffer[w->next_index] = sample;
        w->sum += sample;
    }
    w->next_index = (w->next_index + 1) % w->size;
}

uint16_t get_average(Window *w) {
    if (w->count == 0) return 0;
    return (uint16_t)((w->sum + w->count / 2) / w->count);
}

int main() {
    Window window;
    init_window(&window, 5);
    
    uint16_t samples[] = {1000, 1200, 1100, 1300, 1050, 1250, 1150};
    int sample_count = sizeof(samples) / sizeof(samples[0]);
    uint16_t averages[7];
    
    printf("Sliding Average Filter Algorithm Demo\n");
    printf("Window size: %d\n\n", window.size);
    
    for (int i = 0; i < sample_count; i++) {
        add_sample(&window, samples[i]);
        averages[i] = get_average(&window);
        printf("Add sample %d, current average: %d\n", samples[i], averages[i]);
    }
    
    printf("\nSample | Average\n");
    printf("------ | ------\n");
    for (int i = 0; i < sample_count; i++) {
        printf("%-6d | %-6d\n", samples[i], averages[i]);
    }
    
    free(window.buffer);
    return 0;
}