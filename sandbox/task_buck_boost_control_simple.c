#include <stdio.h>

// 控制器参数
#define KP_V 0.5f    // 电压环比例系数
#define KI_V 0.1f    // 电压环积分系数
#define KP_I 0.3f    // 电流环比例系数

// 限幅参数
#define VOLTAGE_INTEGRAL_LIMIT 5.0f    // 电压环积分限幅
#define CURRENT_REF_MIN 0.1f           // 电流参考最小值
#define CURRENT_REF_MAX 5.0f           // 电流参考最大值
#define DUTY_MIN 0.05f                 // 占空比最小值
#define DUTY_MAX 0.95f                 // 占空比最大值

// 控制器状态变量
typedef struct {
    float voltage_integral;    // 电压环积分项
    float current_ref;         // 电流参考值
    float duty_cycle;          // 当前占空比
} BuckBoostController;

static BuckBoostController controller;

// 限幅函数
static float clamp(float value, float min_val, float max_val) {
    if (value < min_val) return min_val;
    if (value > max_val) return max_val;
    return value;
}

// 初始化控制器
void buck_boost_init(void) {
    controller.voltage_integral = 0.0f;
    controller.current_ref = 0.0f;
    controller.duty_cycle = 0.0f;
}

// 电压环PI控制器
float voltage_pi_controller(float v_ref, float v_meas) {
    float error = v_ref - v_meas;
    
    // 比例项
    float proportional = KP_V * error;
    
    // 积分项
    controller.voltage_integral += KI_V * error;
    controller.voltage_integral = clamp(controller.voltage_integral, 
                                        -VOLTAGE_INTEGRAL_LIMIT, 
                                        VOLTAGE_INTEGRAL_LIMIT);
    
    // 计算电流参考值
    float i_ref = proportional + controller.voltage_integral;
    
    // 电流参考限幅
    controller.current_ref = clamp(i_ref, CURRENT_REF_MIN, CURRENT_REF_MAX);
    
    return controller.current_ref;
}

// 电流环P控制器
float current_p_controller(float i_ref, float i_meas) {
    float error = i_ref - i_meas;
    float duty = controller.duty_cycle + KP_I * error;
    
    // 占空比限幅
    controller.duty_cycle = clamp(duty, DUTY_MIN, DUTY_MAX);
    
    return controller.duty_cycle;
}

// 更新PWM占空比（模拟函数）
void update_pwm_duty(float duty) {
    // 模拟更新PWM寄存器
    // 这里只是模拟，实际硬件会更新PWM寄存器
    (void)duty; // 避免未使用变量警告
}

int main(void) {
    // 初始化控制器
    buck_boost_init();
    
    // 参考电压
    float v_ref = 12.0f;
    
    // 模拟测量值初始化
    float v_meas = 10.0f;  // 初始电压
    float i_meas = 1.0f;   // 初始电流
    
    printf("Buck-Boost变换器双环控制仿真\n");
    printf("周期\tVref(V)\tVmeas(V)\tImeas(A)\tIref(A)\tDuty\n");
    printf("------------------------------------------------------------\n");
    
    // 模拟5个控制周期
    for (int cycle = 1; cycle <= 5; cycle++) {
        // 电压环PI控制器计算电流参考值
        float i_ref = voltage_pi_controller(v_ref, v_meas);
        
        // 电流环P控制器计算占空比
        float duty = current_p_controller(i_ref, i_meas);
        
        // 更新PWM占空比
        update_pwm_duty(duty);
        
        // 打印当前周期结果
        printf("%d\t%.2f\t%.2f\t\t%.2f\t\t%.2f\t%.3f\n", 
               cycle, v_ref, v_meas, i_meas, i_ref, duty);
        
        // 模拟下一个周期的测量值变化
        v_meas += 0.5f;  // 每个周期电压增加0.5V
        i_meas += 0.2f;  // 每个周期电流增加0.2A
    }
    
    return 0;
}