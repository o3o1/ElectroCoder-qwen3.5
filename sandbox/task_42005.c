#include <stdio.h>

float iir_lowpass(float input, float *prev_output, float alpha) {
    float output = alpha * input + (1 - alpha) * (*prev_output);
    *prev_output = output;
    return output;
}

int main() {
    float test_signal[] = {1.0, 1.2, 0.8, 1.5, 0.5, 1.3, 0.7, 1.1, 0.9, 1.0};
    int n = sizeof(test_signal) / sizeof(test_signal[0]);
    
    printf("原始信号: ");
    for (int i = 0; i < n; i++) {
        printf("%.2f ", test_signal[i]);
    }
    printf("\n");
    
    float prev_output1 = test_signal[0];
    printf("alpha=0.3滤波后: %.2f ", prev_output1);
    for (int i = 1; i < n; i++) {
        float filtered = iir_lowpass(test_signal[i], &prev_output1, 0.3f);
        printf("%.2f ", filtered);
    }
    printf("\n");
    
    float prev_output2 = test_signal[0];
    printf("alpha=0.7滤波后: %.2f ", prev_output2);
    for (int i = 1; i < n; i++) {
        float filtered = iir_lowpass(test_signal[i], &prev_output2, 0.7f);
        printf("%.2f ", filtered);
    }
    printf("\n");
    
    return 0;
}