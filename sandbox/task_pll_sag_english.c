#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#define PI 3.1415926535f
#define RATED_AMPLITUDE 311.0f
#define SAG_THRESHOLD 0.8f

bool detect_voltage_sag(float voltages[]) {
    float sum_abs = 0.0f;
    for (int i = 0; i < 3; i++) {
        sum_abs += fabsf(voltages[i]);
    }
    float avg_amplitude = sum_abs / 3.0f;
    return avg_amplitude < (RATED_AMPLITUDE * SAG_THRESHOLD);
}

void compensate_amplitude(float phase_angle, float compensated[]) {
    compensated[0] = RATED_AMPLITUDE * sinf(phase_angle);
    compensated[1] = RATED_AMPLITUDE * sinf(phase_angle - 2.0f * PI / 3.0f);
    compensated[2] = RATED_AMPLITUDE * sinf(phase_angle + 2.0f * PI / 3.0f);
}

int main() {
    printf("PLL Voltage Sag Amplitude Locking Algorithm Test\n");
    printf("Rated Amplitude: %.1fV, Sag Threshold: %.0f%%\n", RATED_AMPLITUDE, SAG_THRESHOLD * 100);
    printf("Sag Detection Threshold: %.1fV\n\n", RATED_AMPLITUDE * SAG_THRESHOLD);
    
    float test_voltages[3][3] = {
        {300.0f, -280.0f, 290.0f},
        {150.0f, -130.0f, 140.0f},
        {248.0f, -250.0f, 249.0f}
    };
    
    float test_phases[3] = {0.5f, 1.2f, 2.0f};
    
    for (int test_case = 0; test_case < 3; test_case++) {
        printf("Test %d:\n", test_case + 1);
        printf("  Input Three-Phase Voltages: [%.1f, %.1f, %.1f] V\n", 
               test_voltages[test_case][0], 
               test_voltages[test_case][1], 
               test_voltages[test_case][2]);
        printf("  Phase Angle: %.2f radians\n", test_phases[test_case]);
        
        bool is_sag = detect_voltage_sag(test_voltages[test_case]);
        printf("  Voltage Sag Detection: %s\n", is_sag ? "Yes" : "No");
        
        if (is_sag) {
            float compensated[3];
            compensate_amplitude(test_phases[test_case], compensated);
            printf("  Compensated Three-Phase Voltages: [%.2f, %.2f, %.2f] V\n", 
                   compensated[0], compensated[1], compensated[2]);
        }
        printf("\n");
    }
    
    return 0;
}