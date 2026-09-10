#include <stdio.h>
#include <math.h>

float calculate_damping_term(float current_freq, float ref_freq, float Kd) {
    return Kd * (current_freq - ref_freq);
}

float apply_damping_control(float* output_power, float damping_term, float max_limit) {
    float new_power = *output_power + damping_term;
    
    if (new_power > max_limit) {
        new_power = max_limit;
    }
    
    if (new_power < 0) {
        new_power = 0;
    }
    
    *output_power = new_power;
    return new_power;
}

int main() {
    struct TestCase {
        float current_freq;
        float ref_freq;
        float Kd;
    };
    
    struct TestCase test_cases[] = {
        {49.5f, 50.0f, 10.0f},
        {50.5f, 50.0f, 8.0f},
        {49.8f, 50.0f, 15.0f},
        {50.2f, 50.0f, 12.0f}
    };
    
    int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);
    
    for (int i = 0; i < num_cases; i++) {
        float current_freq = test_cases[i].current_freq;
        float ref_freq = test_cases[i].ref_freq;
        float Kd = test_cases[i].Kd;
        
        float damping_term = calculate_damping_term(current_freq, ref_freq, Kd);
        
        float output_power = 1000.0f;
        float max_limit = 1500.0f;
        
        float final_power = apply_damping_control(&output_power, damping_term, max_limit);
        
        printf("测试用例%d: 当前频率=%.1fHz, 参考频率=%.1fHz, Kd=%.1f\n", 
               i+1, current_freq, ref_freq, Kd);
        printf("阻尼项 = %.2f\n", damping_term);
        printf("应用阻尼后输出功率 = %.2f\n\n", final_power);
    }
    
    return 0;
}