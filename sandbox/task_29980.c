#include <stdio.h>

int clamp_voltage(float *voltage, float clamp_level) {
    if (*voltage > clamp_level) {
        *voltage = clamp_level;
        return 1;
    }
    return 0;
}

void record_surge_event(int event_id, float voltage, const char* timestamp) {
    printf("Event %d: Surge detected! Voltage=%.2fV, Time=%s\n", event_id, voltage, timestamp);
}

int main() {
    float clamp_level = 5.0f;
    float samples[] = {3.2f, 4.8f, 5.6f, 5.1f, 4.9f, 6.0f};
    int num_samples = sizeof(samples) / sizeof(samples[0]);
    int event_id = 1;
    const char* timestamp = "2024-01-01 12:00:00";
    
    for (int i = 0; i < num_samples; i++) {
        float original_voltage = samples[i];
        int surge_detected = clamp_voltage(&samples[i], clamp_level);
        if (surge_detected) {
            record_surge_event(event_id, original_voltage, timestamp);
            event_id++;
        }
    }
    
    printf("Clamped voltages: ");
    for (int i = 0; i < num_samples; i++) {
        printf("%.2f ", samples[i]);
    }
    printf("\n");
    
    return 0;
}