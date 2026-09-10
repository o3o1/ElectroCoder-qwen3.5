#include <stdio.h>

float delay_compensation(float current_u, float *prev_u) {
    float u_comp = current_u + (current_u - *prev_u);
    *prev_u = current_u;
    return u_comp;
}

int main() {
    float prev_u = 0.0f;
    float input_sequence[] = {1.0f, 2.0f, 1.5f, 3.0f};
    int num_samples = sizeof(input_sequence) / sizeof(input_sequence[0]);
    
    printf("DC/DC变换器一拍延迟补偿算法模拟\n");
    printf("补偿公式: u_comp[k] = u[k] + (u[k] - u[k-1])\n");
    printf("初始状态: u[k-1] = %.3f\n\n", prev_u);
    
    for (int i = 0; i < num_samples; i++) {
        float current_u = input_sequence[i];
        float u_comp = delay_compensation(current_u, &prev_u);
        
        printf("周期%d: u[k]=%.3f, u_comp=%.3f, 更新后u[k-1]=%.3f\n", 
               i + 1, current_u, u_comp, prev_u);
    }
    
    return 0;
}