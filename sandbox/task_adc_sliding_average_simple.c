#include <stdio.h>

#define WINDOW_SIZE 10

int buffer[WINDOW_SIZE];
int index = 0;
int count = 0;
int sum = 0;

int sliding_average_filter(int new_sample) {
    if (count == WINDOW_SIZE) {
        sum -= buffer[index];
    } else {
        count++;
    }
    
    buffer[index] = new_sample;
    sum += new_sample;
    
    index = (index + 1) % WINDOW_SIZE;
    
    return sum / count;
}

int main() {
    int adc_samples[] = {100, 105, 98, 102, 110, 95, 103, 107, 99, 101};
    int sample_count = 10;
    
    printf("Initial 10 samples filtering process:\n");
    for (int i = 0; i < sample_count; i++) {
        int filtered_value = sliding_average_filter(adc_samples[i]);
        printf("Sample: %d, Filtered average: %d\n", adc_samples[i], filtered_value);
    }
    
    int new_samples[] = {104, 96, 108, 97, 105};
    int new_sample_count = 5;
    
    printf("\nAdditional 5 new samples filtering process:\n");
    for (int i = 0; i < new_sample_count; i++) {
        int filtered_value = sliding_average_filter(new_samples[i]);
        printf("Sample: %d, Filtered average: %d\n", new_samples[i], filtered_value);
    }
    
    return 0;
}