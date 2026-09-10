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
    
    printf("=== Three-phase induction motor V/F control algorithm test ===\n\n");
    
    printf("Test 1: Normal V/F region\n");
    printf("Parameters: freq_hz=30Hz, base_freq=50Hz, base_voltage=220V, torque_boost=0%%\n");
    vf_control(30.0f, 50.0f, 220.0f, 0.0f, &u, &v, &w);
    printf("Voltage amplitude: %.2fV\n", 30.0f/50.0f*220.0f);
    printf("U phase voltage: %.2fV\n", u);
    printf("V phase voltage: %.2fV\n", v);
    printf("W phase voltage: %.2fV\n", w);
    printf("\n");
    
    printf("Test 2: Torque boost\n");
    printf("Parameters: freq_hz=5Hz, base_freq=50Hz, base_voltage=220V, torque_boost=20%%\n");
    vf_control(5.0f, 50.0f, 220.0f, 20.0f, &u, &v, &w);
    float base_voltage_5hz = (5.0f/50.0f) * 220.0f;
    float boost_factor = 1.0f + (20.0f/100.0f) * ((10.0f - 5.0f)/10.0f);
    printf("Voltage amplitude: %.2fV (base: %.2fV * boost factor: %.2f)\n", base_voltage_5hz * boost_factor, base_voltage_5hz, boost_factor);
    printf("U phase voltage: %.2fV\n", u);
    printf("V phase voltage: %.2fV\n", v);
    printf("W phase voltage: %.2fV\n", w);
    printf("\n");
    
    printf("Test 3: Field weakening region\n");
    printf("Parameters: freq_hz=60Hz, base_freq=50Hz, base_voltage=220V, torque_boost=0%%\n");
    vf_control(60.0f, 50.0f, 220.0f, 0.0f, &u, &v, &w);
    printf("Voltage amplitude: %.2fV (maintain rated voltage)\n", 220.0f);
    printf("U phase voltage: %.2fV\n", u);
    printf("V phase voltage: %.2fV\n", v);
    printf("W phase voltage: %.2fV\n", w);
    
    return 0;
}