#include <stdio.h>
#include <stdlib.h>

typedef struct {
    float real_voltage;
    float sensor_output;
} CalibrationPoint;

float compensate_voltage(float sensor_value, CalibrationPoint points[], int num_points) {
    if (num_points <= 0) return 0.0f;
    if (num_points == 1) return points[0].real_voltage;
    
    // 检查是否小于最小输出值
    if (sensor_value <= points[0].sensor_output) {
        // 使用第一个校准点进行线性外推（假设斜率为1）
        return points[0].real_voltage + (sensor_value - points[0].sensor_output);
    }
    
    // 检查是否大于最大输出值
    if (sensor_value >= points[num_points-1].sensor_output) {
        // 使用最后两个校准点进行线性外推
        CalibrationPoint p1 = points[num_points-2];
        CalibrationPoint p2 = points[num_points-1];
        float slope = (p2.real_voltage - p1.real_voltage) / (p2.sensor_output - p1.sensor_output);
        return p2.real_voltage + slope * (sensor_value - p2.sensor_output);
    }
    
    // 在区间内查找
    for (int i = 0; i < num_points - 1; i++) {
        if (sensor_value >= points[i].sensor_output && sensor_value <= points[i+1].sensor_output) {
            CalibrationPoint p1 = points[i];
            CalibrationPoint p2 = points[i+1];
            float slope = (p2.real_voltage - p1.real_voltage) / (p2.sensor_output - p1.sensor_output);
            return p1.real_voltage + slope * (sensor_value - p1.sensor_output);
        }
    }
    
    // 不应该到达这里
    return 0.0f;
}

int main() {
    // 定义校准点数组（模拟实际校准数据）
    CalibrationPoint calibration_points[] = {
        {0.0f, 0.02f},     // 真实电压0V，传感器输出0.02V
        {1.0f, 1.05f},     // 真实电压1V，传感器输出1.05V
        {2.0f, 2.08f},     // 真实电压2V，传感器输出2.08V
        {3.0f, 3.12f},     // 真实电压3V，传感器输出3.12V
        {4.0f, 4.15f},     // 真实电压4V，传感器输出4.15V
        {5.0f, 5.20f}      // 真实电压5V，传感器输出5.20V
    };
    
    int num_points = sizeof(calibration_points) / sizeof(calibration_points[0]);
    
    printf("霍尔电压传感器非线性补偿算法测试\n");
    printf("校准点数据：\n");
    for (int i = 0; i < num_points; i++) {
        printf("  真实电压: %.2fV, 传感器输出: %.2fV\n", 
               calibration_points[i].real_voltage, 
               calibration_points[i].sensor_output);
    }
    printf("\n");
    
    // 测试用例1：传感器值在最小校准点以下
    float sensor_value1 = -0.1f;
    float compensated1 = compensate_voltage(sensor_value1, calibration_points, num_points);
    printf("测试用例1 - 传感器值在最小校准点以下:\n");
    printf("  传感器值: %.2fV, 补偿后真实电压: %.2fV\n", sensor_value1, compensated1);
    
    // 测试用例2：传感器值在最大校准点以上
    float sensor_value2 = 5.5f;
    float compensated2 = compensate_voltage(sensor_value2, calibration_points, num_points);
    printf("测试用例2 - 传感器值在最大校准点以上:\n");
    printf("  传感器值: %.2fV, 补偿后真实电压: %.2fV\n", sensor_value2, compensated2);
    
    // 测试用例3：传感器值在校准点区间内
    float sensor_value3 = 2.5f;
    float compensated3 = compensate_voltage(sensor_value3, calibration_points, num_points);
    printf("测试用例3 - 传感器值在校准点区间内:\n");
    printf("  传感器值: %.2fV, 补偿后真实电压: %.2fV\n", sensor_value3, compensated3);
    
    // 额外测试用例：精确匹配校准点
    float sensor_value4 = 3.12f;
    float compensated4 = compensate_voltage(sensor_value4, calibration_points, num_points);
    printf("测试用例4 - 传感器值精确匹配校准点:\n");
    printf("  传感器值: %.2fV, 补偿后真实电压: %.2fV\n", sensor_value4, compensated4);
    
    return 0;
}