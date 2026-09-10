#include <stdio.h>
#include <math.h>

typedef struct {
    float Kp, Ki, Kd;
    float e[3];
    float last_output;
    float output_limit_max;
    float output_limit_min;
} PID_Incremental;

void PID_Incremental_Init(PID_Incremental *pid, float Kp, float Ki, float Kd, float out_max, float out_min) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->e[0] = 0.0f;
    pid->e[1] = 0.0f;
    pid->e[2] = 0.0f;
    pid->last_output = 0.0f;
    pid->output_limit_max = out_max;
    pid->output_limit_min = out_min;
}

float PID_Incremental_Update(PID_Incremental *pid, float target, float measure) {
    pid->e[0] = target - measure;
    
    float delta_u = pid->Kp * (pid->e[0] - pid->e[1]) + 
                    pid->Ki * pid->e[0] + 
                    pid->Kd * (pid->e[0] - 2.0f * pid->e[1] + pid->e[2]);
    
    float output = pid->last_output + delta_u;
    
    if (output > pid->output_limit_max) {
        output = pid->output_limit_max;
    } else if (output < pid->output_limit_min) {
        output = pid->output_limit_min;
    }
    
    pid->e[2] = pid->e[1];
    pid->e[1] = pid->e[0];
    pid->last_output = output;
    
    return output;
}

int main() {
    PID_Incremental pid;
    PID_Incremental_Init(&pid, 1.2f, 0.5f, 0.1f, 10.0f, -10.0f);
    
    float target = 5.0f;
    float measure = 0.0f;
    
    printf("增量式PID控制器测试\n");
    printf("目标值: %.2f\n", target);
    printf("参数: Kp=%.2f, Ki=%.2f, Kd=%.2f\n", pid.Kp, pid.Ki, pid.Kd);
    printf("输出限幅: [%.2f, %.2f]\n\n", pid.output_limit_min, pid.output_limit_max);
    
    for (int i = 0; i < 5; i++) {
        float output = PID_Incremental_Update(&pid, target, measure);
        
        printf("循环 %d:\n", i + 1);
        printf("  测量值: %.4f\n", measure);
        printf("  PID输出: %.4f\n", output);
        
        measure += output * 0.1f;
    }
    
    return 0;
}