#include <stdio.h>
#include <math.h>

double calibrate_signal(double raw_value, double offset, double gain) {
    return (raw_value - offset) * gain;
}

void calibrate_array(double *raw_array, double *calibrated_array, int size, double offset, double gain) {
    for (int i = 0; i < size; i++) {
        calibrated_array[i] = calibrate_signal(raw_array[i], offset, gain);
    }
}

int main() {
    double raw_array[] = {1.2, 2.5, 3.8, 4.1, 5.6};
    int size = sizeof(raw_array) / sizeof(raw_array[0]);
    double calibrated_array[size];
    double offset = 0.5;
    double gain = 2.0;
    
    calibrate_array(raw_array, calibrated_array, size, offset, gain);
    
    printf("Batch calibration results (offset=%.1f, gain=%.1f):\n", offset, gain);
    for (int i = 0; i < size; i++) {
        printf("Raw: %.1f -> Calibrated: %.3f\n", raw_array[i], calibrated_array[i]);
    }
    
    double single_raw = 10.0;
    double single_offset = 1.0;
    double single_gain = 0.5;
    double single_calibrated = calibrate_signal(single_raw, single_offset, single_gain);
    
    printf("\nSingle calibration test:\n");
    printf("Raw: %.1f, offset=%.1f, gain=%.1f -> Calibrated: %.3f\n", 
           single_raw, single_offset, single_gain, single_calibrated);
    
    return 0;
}