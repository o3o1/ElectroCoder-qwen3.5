#include <stdio.h>
#include <stdlib.h>

float scale_voltage(float pt_voltage, float pt_ratio, float input_range_max, float output_range_max) {
    if (input_range_max == 0.0f) {
        return 0.0f;
    }
    return (pt_voltage / input_range_max) * output_range_max;
}

int main() {
    struct test_case {
        float pt_voltage;
        float pt_ratio;
        float input_range_max;
        float output_range_max;
    };
    
    struct test_case test_cases[] = {
        {2.5f, 100.0f, 5.0f, 3.3f},
        {0.0f, 100.0f, 5.0f, 3.3f},
        {5.0f, 100.0f, 5.0f, 3.3f},
        {3.0f, 50.0f, 10.0f, 5.0f}
    };
    
    int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);
    
    for (int i = 0; i < num_cases; i++) {
        struct test_case tc = test_cases[i];
        float scaled_voltage = scale_voltage(tc.pt_voltage, tc.pt_ratio, 
                                           tc.input_range_max, tc.output_range_max);
        
        printf("PT电压: %.2fV, 变比: %.0f, 输入量程: %.2fV, 输出量程: %.2fV -> 适配后: %.2fV\n",
               tc.pt_voltage, tc.pt_ratio, tc.input_range_max, 
               tc.output_range_max, scaled_voltage);
    }
    
    return 0;
}