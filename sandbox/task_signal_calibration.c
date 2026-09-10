#include <stdio.h>
#include <stdlib.h>

typedef struct {
    float gain;
    float offset;
    float min_range;
    float max_range;
} CalibrationParams;

float calibrate_signal(float raw_value, CalibrationParams params) {
    float calibrated = (raw_value + params.offset) * params.gain;
    
    if (calibrated < params.min_range) {
        calibrated = params.min_range;
    }
    if (calibrated > params.max_range) {
        calibrated = params.max_range;
    }
    
    return calibrated;
}

void auto_calibrate(CalibrationParams *params, float *raw_samples, int count) {
    if (count <= 0) {
        return;
    }
    
    float sum = 0.0f;
    for (int i = 0; i < count; i++) {
        sum += raw_samples[i];
    }
    float average = sum / count;
    
    params->offset = -average;
    params->gain = 1.0f;
}

int main() {
    CalibrationParams params = {2.0f, -0.5f, -10.0f, 10.0f};
    
    float test_values[] = {-1.0f, 0.0f, 1.0f, 5.0f, -6.0f};
    int test_count = sizeof(test_values) / sizeof(test_values[0]);
    
    printf("Manual calibration test:\n");
    for (int i = 0; i < test_count; i++) {
        float calibrated = calibrate_signal(test_values[i], params);
        printf("Original: %.6f -> Calibrated: %.6f\n", test_values[i], calibrated);
    }
    
    float samples[] = {0.1f, 0.2f, -0.1f, 0.3f, -0.2f};
    int sample_count = sizeof(samples) / sizeof(samples[0]);
    
    CalibrationParams auto_params = {1.0f, 0.0f, -10.0f, 10.0f};
    auto_calibrate(&auto_params, samples, sample_count);
    
    printf("\nAuto calibration parameters: gain=%.6f, offset=%.6f\n", 
           auto_params.gain, auto_params.offset);
    
    return 0;
}