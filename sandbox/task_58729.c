#include <stdio.h>

float iir_lowpass(float x, float *prev_y, float alpha) {
    float y = alpha * x + (1 - alpha) * (*prev_y);
    *prev_y = y;
    return y;
}

int main() {
    float input[] = {1.0, 0.5, 0.2, 0.8, 0.3, 0.9, 0.1, 0.7};
    int n = sizeof(input) / sizeof(input[0]);
    float alpha = 0.3;
    float prev_y = 0.0;
    float sum = 0.0;
    
    for (int i = 0; i < n; i++) {
        float output = iir_lowpass(input[i], &prev_y, alpha);
        printf("Input[%d]=%.2f, Output[%d]=%.4f\n", i, input[i], i, output);
        sum += output;
    }
    
    float average = sum / n;
    printf("Average output: %.4f\n", average);
    
    return 0;
}