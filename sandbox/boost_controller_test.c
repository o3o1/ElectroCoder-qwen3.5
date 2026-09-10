#include <stdio.h>

// 模拟硬件寄存器宏定义
#define ADC_READ_VOUT()  (10.5f)  // 模拟输出电压测量值
#define ADC_READ_VIN()   (5.0f)   // 模拟输入电压测量值
#define SET_PWM_DUTY(duty)  // 模拟设置PWM占空比

// PI控制器参数
#define KP 0.05f
#define KI 0.001f
#define INTEGRAL_LIMIT_MAX 10.0f
#define INTEGRAL_LIMIT_MIN -10.0f
#define DUTY_LIMIT_MAX 0.85f
#define DUTY_LIMIT_MIN 0.05f

// PI控制器状态变量
static float integral_sum = 0.0f;

// PI控制器函数
float pi_controller(float error) {
    // 比例项
    float proportional = KP * error;
    
    // 积分项
    integral_sum += KI * error;
    
    // 积分抗饱和限幅
    if (integral_sum > INTEGRAL_LIMIT_MAX) {
        integral_sum = INTEGRAL_LIMIT_MAX;
    } else if (integral_sum < INTEGRAL_LIMIT_MIN) {
        integral_sum = INTEGRAL_LIMIT_MIN;
    }
    
    // 计算输出占空比
    float duty = proportional + integral_sum;
    
    // 占空比限幅
    if (duty > DUTY_LIMIT_MAX) {
        duty = DUTY_LIMIT_MAX;
    } else if (duty < DUTY_LIMIT_MIN) {
        duty = DUTY_LIMIT_MIN;
    }
    
    return duty;
}

int main() {
    // 初始化参考电压
    const float Vref = 12.0f;
    
    printf("Boost升压转换器电压控制算法测试\n");
    printf("参考电压 Vref = %.2f V\n", Vref);
    printf("PI参数: Kp = %.3f, Ki = %.3f\n", KP, KI);
    printf("占空比限制: [%.2f, %.2f]\n", DUTY_LIMIT_MIN, DUTY_LIMIT_MAX);
    printf("积分项限制: [%.1f, %.1f]\n", INTEGRAL_LIMIT_MIN, INTEGRAL_LIMIT_MAX);
    printf("========================================\n");
    
    // 模拟5个控制周期
    for (int cycle = 1; cycle <= 5; cycle++) {
        // 读取模拟ADC值
        float Vout_measured = ADC_READ_VOUT();
        float Vin_measured = ADC_READ_VIN();
        
        // 计算误差
        float error = Vref - Vout_measured;
        
        // 调用PI控制器计算占空比
        float duty = pi_controller(error);
        
        // 设置PWM占空比
        SET_PWM_DUTY(duty);
        
        // 输出当前周期信息
        printf("周期 %d:\n", cycle);
        printf("  输入电压 Vin = %.2f V\n", Vin_measured);
        printf("  输出电压 Vout = %.2f V\n", Vout_measured);
        printf("  误差 error = %.2f V\n", error);
        printf("  计算占空比 duty = %.4f\n", duty);
        printf("  积分累积值 = %.4f\n", integral_sum);
    }
    
    printf("========================================\n");
    printf("测试完成\n");
    
    return 0;
}