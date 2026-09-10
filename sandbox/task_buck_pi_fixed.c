#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
    float kp;
    float ki;
    float integral;
    float integral_limit;
    int compensation_delay;
    float* compensation_buffer;
    int buffer_index;
} BuckPIController;

void buck_pi_init(BuckPIController* ctrl, float kp, float ki, float integral_limit, int delay_cycles) {
    ctrl->kp = kp;
    ctrl->ki = ki;
    ctrl->integral = 0.0f;
    ctrl->integral_limit = integral_limit;
    ctrl->compensation_delay = delay_cycles;
    
    if (delay_cycles > 0) {
        ctrl->compensation_buffer = (float*)malloc(delay_cycles * sizeof(float));
        memset(ctrl->compensation_buffer, 0, delay_cycles * sizeof(float));
    } else {
        ctrl->compensation_buffer = NULL;
    }
    
    ctrl->buffer_index = 0;
}

float buck_pi_update(BuckPIController* ctrl, float v_ref, float v_fb, float dt) {
    float error = v_ref - v_fb;
    
    ctrl->integral += ctrl->ki * error * dt;
    
    if (ctrl->integral > ctrl->integral_limit) {
        ctrl->integral = ctrl->integral_limit;
    } else if (ctrl->integral < -ctrl->integral_limit) {
        ctrl->integral = -ctrl->integral_limit;
    }
    
    float proportional = ctrl->kp * error;
    float output = proportional + ctrl->integral;
    
    if (ctrl->compensation_delay > 0) {
        ctrl->compensation_buffer[ctrl->buffer_index] = output;
        ctrl->buffer_index = (ctrl->buffer_index + 1) % ctrl->compensation_delay;
        
        int delayed_index = (ctrl->buffer_index + ctrl->compensation_delay - 1) % ctrl->compensation_delay;
        return ctrl->compensation_buffer[delayed_index];
    }
    
    return output;
}

void buck_pi_cleanup(BuckPIController* ctrl) {
    if (ctrl->compensation_buffer != NULL) {
        free(ctrl->compensation_buffer);
        ctrl->compensation_buffer = NULL;
    }
}

int main() {
    BuckPIController ctrl;
    buck_pi_init(&ctrl, 0.5f, 10.0f, 1.0f, 3);
    
    float dt = 0.001f;
    float v_ref_values[] = {5.0f, 5.0f, 5.0f, 5.0f, 5.0f};
    float v_fb_values[] = {4.5f, 4.8f, 4.9f, 5.0f, 5.1f};
    
    printf("Buck变换器电压闭环PI控制仿真\n");
    printf("参数: kp=%.1f, ki=%.1f, 积分限幅=%.1f, 延迟补偿=%d周期\n\n", 
           ctrl.kp, ctrl.ki, ctrl.integral_limit, ctrl.compensation_delay);
    printf("周期\tv_ref\tv_fb\t误差\t\t积分值\t\t补偿前输出\t补偿后输出\n");
    printf("-----------------------------------------------------------------------------\n");
    
    for (int i = 0; i < 5; i++) {
        float v_ref = v_ref_values[i];
        float v_fb = v_fb_values[i];
        float error = v_ref - v_fb;
        float integral_before = ctrl.integral;
        
        float output_before = ctrl.kp * error + integral_before;
        float output_compensated = buck_pi_update(&ctrl, v_ref, v_fb, dt);
        
        printf("%d\t%.1f\t%.1f\t%.3f\t\t%.6f\t%.6f\t%.6f\n", 
               i+1, v_ref, v_fb, error, ctrl.integral, output_before, output_compensated);
    }
    
    buck_pi_cleanup(&ctrl);
    return 0;
}