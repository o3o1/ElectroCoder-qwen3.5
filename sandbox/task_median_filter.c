#include <stdio.h>
#include <stdlib.h>

#define ADC_VALUE (*((volatile int*)0x40012400))
volatile int mock_adc_value = 0;

void bubble_sort(int* arr, int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

int median_filter(int* buffer, int window_size) {
    int* temp = (int*)malloc(window_size * sizeof(int));
    for (int i = 0; i < window_size; i++) {
        temp[i] = buffer[i];
    }
    
    bubble_sort(temp, window_size);
    
    int median = temp[window_size / 2];
    free(temp);
    return median;
}

int main() {
    printf("=== Median Filter Test ===\n\n");
    
    printf("Test Case 1: Window Size 5\n");
    int test_data1[] = {10, 12, 11, 100, 13};
    int window_size1 = 5;
    
    printf("Original data: ");
    for (int i = 0; i < window_size1; i++) {
        printf("%d ", test_data1[i]);
    }
    printf("\n");
    
    int result1 = median_filter(test_data1, window_size1);
    printf("Filtered median: %d (expected: 11)\n\n", result1);
    
    printf("Test Case 2: Window Size 7\n");
    int test_data2[] = {15, 17, 16, 200, 18, 19, 14};
    int window_size2 = 7;
    
    printf("Original data: ");
    for (int i = 0; i < window_size2; i++) {
        printf("%d ", test_data2[i]);
    }
    printf("\n");
    
    int result2 = median_filter(test_data2, window_size2);
    printf("Filtered median: %d\n", result2);
    
    printf("\n=== Test Complete ===\n");
    return 0;
}