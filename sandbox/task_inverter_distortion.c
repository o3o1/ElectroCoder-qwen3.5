#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define PI 3.14159265358979323846

typedef struct {
    unsigned long timestamp;
    float voltage;
    float frequency;
} InverterSample;

int detect_distortion(InverterSample samples[], int n, float threshold) {
    if (n <= 0) return 0;
    
    float Vm = 0.0;
    for (int i = 0; i < n; i++) {
        float abs_voltage = fabsf(samples[i].voltage);
        if (abs_voltage > Vm) {
            Vm = abs_voltage;
        }
    }
    
    if (Vm < 0.001) return 0;
    
    float f = samples[0].frequency;
    float phi = asinf(samples[0].voltage / Vm);
    
    int distortion_count = 0;
    
    for (int i = 0; i < n; i++) {
        float t = samples[i].timestamp / 1000.0;
        float theoretical = Vm * sinf(2 * PI * f * t + phi);
        float actual = samples[i].voltage;
        float deviation = fabsf(actual - theoretical);
        
        if (deviation > threshold) {
            distortion_count++;
        }
    }
    
    return distortion_count;
}

void generate_normal_waveform(InverterSample samples[], int n, float frequency, float peak_voltage) {
    float time_step = 1000.0 / (frequency * 20);
    
    for (int i = 0; i < n; i++) {
        samples[i].timestamp = i * time_step;
        samples[i].frequency = frequency;
        float t = samples[i].timestamp / 1000.0;
        samples[i].voltage = peak_voltage * sinf(2 * PI * frequency * t);
    }
}

void generate_distorted_waveform(InverterSample samples[], int n, float frequency, float peak_voltage) {
    srand(time(NULL));
    
    float time_step = 1000.0 / (frequency * 20);
    
    for (int i = 0; i < n; i++) {
        samples[i].timestamp = i * time_step;
        samples[i].frequency = frequency;
        float t = samples[i].timestamp / 1000.0;
        float ideal = peak_voltage * sinf(2 * PI * frequency * t);
        
        if (rand() % 10 < 3) {
            float distortion = (rand() % 100) / 100.0 * peak_voltage * 0.3;
            if (rand() % 2 == 0) {
                samples[i].voltage = ideal + distortion;
            } else {
                samples[i].voltage = ideal - distortion;
            }
        } else {
            samples[i].voltage = ideal;
        }
    }
}

int main() {
    const int sample_count = 200;
    float threshold = 10.0;
    
    printf("Inverter Voltage Distortion Detection Algorithm Test\n");
    printf("====================================================\n\n");
    
    InverterSample normal_samples[sample_count];
    InverterSample distorted_samples[sample_count];
    
    generate_normal_waveform(normal_samples, sample_count, 50.0, 311.0);
    generate_distorted_waveform(distorted_samples, sample_count, 50.0, 311.0);
    
    printf("Test Case 1: Normal Waveform (50Hz, Peak 311V)\n");
    int normal_distortion = detect_distortion(normal_samples, sample_count, threshold);
    printf("Total Samples: %d\n", sample_count);
    printf("Distortion Points: %d\n", normal_distortion);
    printf("Distortion Ratio: %.2f%%\n\n", (float)normal_distortion / sample_count * 100);
    
    printf("Test Case 2: Distorted Waveform (50Hz, Peak 311V)\n");
    int distorted_distortion = detect_distortion(distorted_samples, sample_count, threshold);
    printf("Total Samples: %d\n", sample_count);
    printf("Distortion Points: %d\n", distorted_distortion);
    printf("Distortion Ratio: %.2f%%\n\n", (float)distorted_distortion / sample_count * 100);
    
    printf("Test Case 3: Severe Distortion Waveform\n");
    InverterSample severe_samples[sample_count];
    generate_normal_waveform(severe_samples, sample_count, 50.0, 311.0);
    for (int i = 50; i < 100; i++) {
        severe_samples[i].voltage = severe_samples[i].voltage * 0.5;
    }
    for (int i = 150; i < 180; i++) {
        severe_samples[i].voltage = severe_samples[i].voltage + 100.0;
    }
    int severe_distortion = detect_distortion(severe_samples, sample_count, threshold);
    printf("Total Samples: %d\n", sample_count);
    printf("Distortion Points: %d\n", severe_distortion);
    printf("Distortion Ratio: %.2f%%\n\n", (float)severe_distortion / sample_count * 100);
    
    printf("Threshold Setting: %.2f V\n", threshold);
    printf("Detection Complete\n");
    
    return 0;
}