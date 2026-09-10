#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define REF_CURRENT 5.0f
#define REF_RAW_VALUE 2048
#define ADC_RESOLUTION 4096
#define ADC_REF_VOLTAGE 3.3f
#define SENSE_RESISTOR 0.1f

void calibrate_current_sensor(int16_t raw_samples[], int n, int16_t *offset, float *gain) {
    int32_t sum = 0;
    
    for (int i = 0; i < n; i++) {
        sum += raw_samples[i];
    }
    
    *offset = (int16_t)(sum / n);
    
    float ref_minus_offset = (float)REF_RAW_VALUE - (float)(*offset);
    if (ref_minus_offset != 0.0f) {
        *gain = REF_CURRENT / ref_minus_offset;
    } else {
        *gain = 1.0f;
    }
}

int main() {
    srand(time(NULL));
    
    const int n = 10;
    int16_t raw_samples[n];
    
    printf("=== 反激变换器电流采样校准算法 ===\n\n");
    
    printf("模拟零电流时的ADC采样数据（-100到+100之间的随机噪声）：\n");
    for (int i = 0; i < n; i++) {
        raw_samples[i] = (int16_t)(rand() % 201 - 100);
        printf("raw_samples[%d] = %d\n", i, raw_samples[i]);
    }
    
    int16_t offset;
    float gain;
    
    calibrate_current_sensor(raw_samples, n, &offset, &gain);
    
    printf("\n校准结果：\n");
    printf("零电流偏移量 offset = %d\n", offset);
    printf("增益系数 gain = %.6f A/ADC\n", gain);
    
    int16_t test_raw_value = 3000;
    float calibrated_current = (float)(test_raw_value - offset) * gain;
    
    printf("\n校准验证：\n");
    printf("测试采样值 = %d\n", test_raw_value);
    printf("校准后电流 = (测试采样值 - offset) × gain\n");
    printf("校准后电流 = (%d - %d) × %.6f\n", test_raw_value, offset, gain);
    printf("校准后电流 = %.6f A\n", calibrated_current);
    
    printf("\n参考信息：\n");
    printf("参考电流 = %.1f A\n", REF_CURRENT);
    printf("参考采样值 = %d\n", REF_RAW_VALUE);
    printf("ADC分辨率 = 12位（0-%d）\n", ADC_RESOLUTION - 1);
    printf("ADC参考电压 = %.1f V\n", ADC_REF_VOLTAGE);
    printf("采样电阻 = %.1f Ω\n", SENSE_RESISTOR);
    
    return 0;
}