#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void vf_control(float freq_hz, float base_freq, float base_voltage, float torque_boost_percent, float *voltage_u, float *voltage_v, float *voltage_w) {
    float voltage_amplitude;
    
    if (freq_hz <= base_freq) {
        voltage_amplitude = (freq_hz / base_freq) * base_voltage;
    } else {
        voltage_amplitude = base_voltage;
    }
    
    if (freq_hz < 10.0f) {
        float boost_factor = 1.0f + (torque_boost_percent / 100.0f) * ((10.0f - freq_hz) / 10.0f);
        voltage_amplitude *= boost_factor;
    }
    
    float angle = 0.0f;
    float phase_angle_u = angle;
    float phase_angle_v = angle - 2.0f * M_PI / 3.0f;
    float phase_angle_w = angle + 2.0f * M_PI / 3.0f;
    
    *voltage_u = voltage_amplitude * sin(phase_angle_u);
    *voltage_v = voltage_amplitude * sin(phase_angle_v);
    *voltage_w = voltage_amplitude * sin(phase_angle_w);
}

int main() {
    float u, v, w;
    
    printf("Test 1: Normal V/F region\n");
    vf_control(30.0f, 50.0f, 220.0f, 0.0f, &u, &v, &w);
    printf("U: %.2fV, V: %.2fV, W: %.2fV\n", u, v, w);
    
    printf("\nTest 2: Torque boost\n");
    vf_control(5.0f, 50.0f, 220.0f, 20.0f, &u, &v, &w);
    printf("U: %.2fV, V: %.2fV, W: %.2fV\n", u, v, w);
    
    printf("\nTest 3: Field weakening region\n");
    vf_control(60.0f, 50.0f, 220.0f, 0.0f, &u, &v, &w);
    printf("U: %.2fV, V: %.2fV, W: %.2fV\n", u, v, w);
    
    return 0;
}