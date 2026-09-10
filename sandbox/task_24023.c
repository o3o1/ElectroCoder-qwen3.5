#include <stdio.h>
#include <math.h>

typedef struct {
    float kp;
    float ki;
    float kd;
    float ts;
    float e1;
    float e2;
    float u1;
    float u2;
} PID_Tustin;

void pid_tustin_init(PID_Tustin *pid, float kp, float ki, float kd, float ts) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->ts = ts;
    pid->e1 = 0.0f;
    pid->e2 = 0.0f;
    pid->u1 = 0.0f;
    pid->u2 = 0.0f;
}

float pid_tustin_update(PID_Tustin *pid, float setpoint, float measurement) {
    float e0 = setpoint - measurement;
    
    float a0 = pid->kp + pid->ki * pid->ts / 2.0f + 2.0f * pid->kd / pid->ts;
    float a1 = pid->ki * pid->ts - 4.0f * pid->kd / pid->ts;
    float a2 = -pid->kp + pid->ki * pid->ts / 2.0f + 2.0f * pid->kd / pid->ts;
    
    float u0 = pid->u2 + a0 * e0 + a1 * pid->e1 + a2 * pid->e2;
    
    pid->e2 = pid->e1;
    pid->e1 = e0;
    pid->u2 = pid->u1;
    pid->u1 = u0;
    
    return u0;
}

int main() {
    printf("=== Tustin离散PID控制器测试 ===\n\n");
    
    PID_Tustin pid1, pid2, pid3;
    
    printf("测试用例1: 纯比例控制 (kp=1.0, ki=0.0, kd=0.0)\n");
    printf("步数\t设定值\t测量值\t控制输出\n");
    pid_tustin_init(&pid1, 1.0f, 0.0f, 0.0f, 0.01f);
    float measurement1 = 0.0f;
    for (int i = 0; i < 20; i++) {
        float setpoint = (i >= 5) ? 10.0f : 0.0f;
        float output = pid_tustin_update(&pid1, setpoint, measurement1);
        measurement1 += output * 0.01f;
        printf("%d\t%.2f\t%.2f\t%.2f\n", i, setpoint, measurement1, output);
    }
    
    printf("\n测试用例2: PI控制 (kp=2.0, ki=5.0, kd=0.0)\n");
    printf("步数\t设定值\t测量值\t控制输出\n");
    pid_tustin_init(&pid2, 2.0f, 5.0f, 0.0f, 0.01f);
    float measurement2 = 0.0f;
    for (int i = 0; i < 30; i++) {
        float setpoint = (i >= 10) ? 5.0f : 0.0f;
        float output = pid_tustin_update(&pid2, setpoint, measurement2);
        measurement2 += output * 0.01f;
        printf("%d\t%.2f\t%.2f\t%.2f\n", i, setpoint, measurement2, output);
    }
    
    printf("\n测试用例3: PID控制 (kp=3.0, ki=8.0, kd=0.5)\n");
    printf("步数\t设定值\t测量值\t控制输出\n");
    pid_tustin_init(&pid3, 3.0f, 8.0f, 0.5f, 0.01f);
    float measurement3 = 0.0f;
    for (int i = 0; i < 40; i++) {
        float setpoint = (i >= 15) ? 8.0f : 0.0f;
        float output = pid_tustin_update(&pid3, setpoint, measurement3);
        measurement3 += output * 0.01f;
        printf("%d\t%.2f\t%.2f\t%.2f\n", i, setpoint, measurement3, output);
    }
    
    return 0;
}