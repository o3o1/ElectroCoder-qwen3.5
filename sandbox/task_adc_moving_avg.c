#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int* data;
    int size;
    int count;
    int sum;
    int next_index;
} CircularBuffer;

void initBuffer(CircularBuffer* buf, int window_size) {
    buf->data = (int*)malloc(window_size * sizeof(int));
    buf->size = window_size;
    buf->count = 0;
    buf->sum = 0;
    buf->next_index = 0;
}

void addSample(CircularBuffer* buf, int sample) {
    if (buf->count < buf->size) {
        buf->data[buf->next_index] = sample;
        buf->sum += sample;
        buf->count++;
    } else {
        int oldest_index = (buf->next_index + 1) % buf->size;
        buf->sum -= buf->data[oldest_index];
        buf->data[buf->next_index] = sample;
        buf->sum += sample;
    }
    buf->next_index = (buf->next_index + 1) % buf->size;
}

int getMovingAverage(CircularBuffer* buf) {
    if (buf->count == 0) return 0;
    return buf->sum / buf->count;
}

void freeBuffer(CircularBuffer* buf) {
    free(buf->data);
    buf->data = NULL;
    buf->size = 0;
    buf->count = 0;
    buf->sum = 0;
    buf->next_index = 0;
}

int main() {
    CircularBuffer buffer;
    initBuffer(&buffer, 5);
    
    int samples[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    int num_samples = sizeof(samples) / sizeof(samples[0]);
    
    for (int i = 0; i < num_samples; i++) {
        addSample(&buffer, samples[i]);
        int avg = getMovingAverage(&buffer);
        printf("Sample[%d]=%d, Average=%d\n", i+1, samples[i], avg);
    }
    
    freeBuffer(&buffer);
    return 0;
}