#include <stdio.h>
#include <math.h>

typedef struct {
    float Kp;
    float Ki;
    float Kd;
    float epsilon;
    float last_error;
    float integral;
    float last_measurement;
} PID_Controller;

void PID_Init(PID_Controller *pid, float Kp, float Ki, float Kd, float epsilon) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->epsilon = epsilon;
    pid->last_error = 0.0f;
    pid->integral = 0.0f;
    pid->last_measurement = 0.0f;
}

float PID_Calculate(PID_Controller *pid, float setpoint, float measurement) {
    float error = setpoint - measurement;
    
    if (fabsf(error) > pid->epsilon) {
        pid->integral = 0.0f;
    } else {
        pid->integral += error;
    }
    
    float derivative = measurement - pid->last_measurement;
    
    float output = pid->Kp * error + pid->Ki * pid->integral - pid->Kd * derivative;
    
    pid->last_error = error;
    pid->last_measurement = measurement;
    
    return output;
}

int main() {
    PID_Controller pid;
    PID_Init(&pid, 1.2f, 0.5f, 0.3f, 10.0f);
    
    float test_setpoints[] = {100.0f, 100.0f, 100.0f, 100.0f, 100.0f};
    float test_measurements[] = {80.0f, 85.0f, 95.0f, 99.0f, 100.5f};
    
    printf("周期号 | 设定值 | 测量值 | 控制输出\n");
    printf("------------------------------------\n");
    
    for (int i = 0; i < 5; i++) {
        float output = PID_Calculate(&pid, test_setpoints[i], test_measurements[i]);
        printf("%6d | %6.1f | %6.1f | %8.2f\n", 
               i + 1, test_setpoints[i], test_measurements[i], output);
    }
    
    return 0;
}