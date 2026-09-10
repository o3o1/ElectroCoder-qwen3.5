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
    
    printf("=== Flyback Current Sensor Calibration ===\n\n");
    
    printf("Simulated ADC samples at zero current (-100 to +100 random noise):\n");
    for (int i = 0; i < n; i++) {
        raw_samples[i] = (int16_t)(rand() % 201 - 100);
        printf("raw_samples[%d] = %d\n", i, raw_samples[i]);
    }
    
    int16_t offset;
    float gain;
    
    calibrate_current_sensor(raw_samples, n, &offset, &gain);
    
    printf("\nCalibration Results:\n");
    printf("Zero current offset = %d\n", offset);
    printf("Gain coefficient = %.6f A/ADC\n", gain);
    
    int16_t test_raw_value = 3000;
    float calibrated_current = (float)(test_raw_value - offset) * gain;
    
    printf("\nCalibration Verification:\n");
    printf("Test sample value = %d\n", test_raw_value);
    printf("Calibrated current = (Test sample - offset) * gain\n");
    printf("Calibrated current = (%d - %d) * %.6f\n", test_raw_value, offset, gain);
    printf("Calibrated current = %.6f A\n", calibrated_current);
    
    printf("\nReference Information:\n");
    printf("Reference current = %.1f A\n", REF_CURRENT);
    printf("Reference sample value = %d\n", REF_RAW_VALUE);
    printf("ADC resolution = 12-bit (0-%d)\n", ADC_RESOLUTION - 1);
    printf("ADC reference voltage = %.1f V\n", ADC_REF_VOLTAGE);
    printf("Sense resistor = %.1f Ohm\n", SENSE_RESISTOR);
    
    return 0;
}