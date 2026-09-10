#include <stdio.h>

typedef struct {
    float wind_speed;
    float power;
} PowerCurvePoint;

float lookup_power(PowerCurvePoint* curve, int size, float wind_speed) {
    if (size <= 0) return 0.0f;
    
    if (wind_speed < curve[0].wind_speed) {
        return 0.0f;
    }
    
    if (wind_speed > curve[size-1].wind_speed) {
        return curve[size-1].power;
    }
    
    for (int i = 0; i < size; i++) {
        if (wind_speed == curve[i].wind_speed) {
            return curve[i].power;
        }
        
        if (i < size-1 && wind_speed > curve[i].wind_speed && wind_speed < curve[i+1].wind_speed) {
            float ratio = (wind_speed - curve[i].wind_speed) / (curve[i+1].wind_speed - curve[i].wind_speed);
            return curve[i].power + ratio * (curve[i+1].power - curve[i].power);
        }
    }
    
    return curve[size-1].power;
}

int main() {
    PowerCurvePoint curve[] = {
        {3.0, 15.5},
        {4.0, 42.0},
        {5.0, 82.5},
        {6.0, 138.0},
        {7.0, 210.0}
    };
    int size = sizeof(curve) / sizeof(curve[0]);
    
    float test_speeds[] = {2.5, 4.3, 6.0, 8.0};
    int test_count = sizeof(test_speeds) / sizeof(test_speeds[0]);
    
    for (int i = 0; i < test_count; i++) {
        float power = lookup_power(curve, size, test_speeds[i]);
        printf("风速 %.1f m/s -> 功率 %.1f kW\n", test_speeds[i], power);
    }
    
    return 0;
}