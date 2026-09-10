#include <stdio.h>
#include <stdlib.h>

typedef struct {
    float real_voltage;
    float sensor_output;
} CalibrationPoint;

float compensate_voltage(float sensor_value, CalibrationPoint points[], int num_points) {
    if (num_points <= 0) return 0.0f;
    if (num_points == 1) return points[0].real_voltage;
    
    if (sensor_value <= points[0].sensor_output) {
        return points[0].real_voltage + (sensor_value - points[0].sensor_output);
    }
    
    if (sensor_value >= points[num_points-1].sensor_output) {
        CalibrationPoint p1 = points[num_points-2];
        CalibrationPoint p2 = points[num_points-1];
        float slope = (p2.real_voltage - p1.real_voltage) / (p2.sensor_output - p1.sensor_output);
        return p2.real_voltage + slope * (sensor_value - p2.sensor_output);
    }
    
    for (int i = 0; i < num_points - 1; i++) {
        if (sensor_value >= points[i].sensor_output && sensor_value <= points[i+1].sensor_output) {
            CalibrationPoint p1 = points[i];
            CalibrationPoint p2 = points[i+1];
            float slope = (p2.real_voltage - p1.real_voltage) / (p2.sensor_output - p1.sensor_output);
            return p1.real_voltage + slope * (sensor_value - p1.sensor_output);
        }
    }
    
    return 0.0f;
}

int main() {
    CalibrationPoint calibration_points[] = {
        {0.0f, 0.02f},
        {1.0f, 1.05f},
        {2.0f, 2.08f},
        {3.0f, 3.12f},
        {4.0f, 4.15f},
        {5.0f, 5.20f}
    };
    
    int num_points = sizeof(calibration_points) / sizeof(calibration_points[0]);
    
    printf("Hall Voltage Sensor Nonlinear Compensation Algorithm Test\n");
    printf("Calibration Points:\n");
    for (int i = 0; i < num_points; i++) {
        printf("  Real Voltage: %.2fV, Sensor Output: %.2fV\n", 
               calibration_points[i].real_voltage, 
               calibration_points[i].sensor_output);
    }
    printf("\n");
    
    float sensor_value1 = -0.1f;
    float compensated1 = compensate_voltage(sensor_value1, calibration_points, num_points);
    printf("Test Case 1 - Sensor value below minimum calibration point:\n");
    printf("  Sensor Value: %.2fV, Compensated Voltage: %.2fV\n", sensor_value1, compensated1);
    
    float sensor_value2 = 5.5f;
    float compensated2 = compensate_voltage(sensor_value2, calibration_points, num_points);
    printf("Test Case 2 - Sensor value above maximum calibration point:\n");
    printf("  Sensor Value: %.2fV, Compensated Voltage: %.2fV\n", sensor_value2, compensated2);
    
    float sensor_value3 = 2.5f;
    float compensated3 = compensate_voltage(sensor_value3, calibration_points, num_points);
    printf("Test Case 3 - Sensor value within calibration range:\n");
    printf("  Sensor Value: %.2fV, Compensated Voltage: %.2fV\n", sensor_value3, compensated3);
    
    float sensor_value4 = 3.12f;
    float compensated4 = compensate_voltage(sensor_value4, calibration_points, num_points);
    printf("Test Case 4 - Sensor value exactly matches calibration point:\n");
    printf("  Sensor Value: %.2fV, Compensated Voltage: %.2fV\n", sensor_value4, compensated4);
    
    return 0;
}