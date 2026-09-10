#include <stdio.h>
#include <math.h>

// 系统参数
#define VIN 24.0
#define VREF -12.0
#define I_MAX 5.0
#define FSW 100000.0
#define T (1.0/FSW)
#define KP_V 0.05
#define KI_V 2.0
#define KP_I 0.3
#define RLOAD 6.0
#define L 100e-6
#define C 470e-6

// 系统状态结构体
typedef struct {
    double vout;        // 输出电压
    double il;          // 电感电流
    double integral_v;  // 电压环积分项
} SystemState;

// 电压环PI控制器
double voltage_pi_controller(double error, double dt, double *integral) {
    *integral += error * dt;
    return KP_V * error + KI_V * (*integral);
}

// 电流环P控制器
double current_p_controller(double error) {
    return KP_I * error;
}

// 根据占空比更新电路状态（前向欧拉法）
void simulate_one_step(double duty, SystemState *state) {
    // Buck-Boost变换器状态方程
    // dIL/dt = (Vin*duty - |Vout|*(1-duty))/L
    // dVout/dt = (-IL*(1-duty) - Vout/Rload)/C
    
    double dIL_dt = (VIN * duty - fabs(state->vout) * (1.0 - duty)) / L;
    double dVout_dt = (-state->il * (1.0 - duty) - state->vout / RLOAD) / C;
    
    state->il += dIL_dt * T;
    state->vout += dVout_dt * T;
}

int main() {
    SystemState state = {0.0, 0.0, 0.0};  // 初始状态
    double duty = 0.333;  // 初始占空比设为理论值
    
    printf("Buck-Boost变换器双环控制仿真\n");
    printf("==============================\n");
    printf("输入电压: %.1f V\n", VIN);
    printf("目标输出电压: %.1f V\n", VREF);
    printf("开关频率: %.0f Hz\n", FSW);
    printf("控制周期: %.2f us\n", T*1e6);
    printf("电压环PI: Kp=%.2f, Ki=%.1f\n", KP_V, KI_V);
    printf("电流环P: Kp=%.1f\n", KP_I);
    printf("负载电阻: %.1f Ω\n", RLOAD);
    printf("\n");
    
    printf("周期序号\tVout(V)\t\tIL(A)\t\t占空比\n");
    printf("-------------------------------------------------\n");
    
    // 模拟1000个控制周期（10ms实时）
    for (int i = 0; i < 1000; i++) {
        // 1. 计算电压误差 → 电压环输出（作为电流参考值）
        double v_error = VREF - state.vout;
        double i_ref = voltage_pi_controller(v_error, T, &state.integral_v);
        
        // 2. 电流限幅保护
        if (i_ref > I_MAX) i_ref = I_MAX;
        if (i_ref < -I_MAX) i_ref = -I_MAX;
        
        // 3. 计算电流误差 → 电流环输出（占空比）
        double i_error = i_ref - state.il;
        double duty_adjust = current_p_controller(i_error);
        duty += duty_adjust * T;  // 积分调整占空比
        
        // 4. 占空比限幅0-1
        if (duty > 1.0) duty = 1.0;
        if (duty < 0.0) duty = 0.0;
        
        // 5. 调用simulate_one_step更新电路状态
        simulate_one_step(duty, &state);
        
        // 每100个周期打印一次
        if (i % 100 == 0) {
            printf("%d\t\t%.3f\t\t%.3f\t\t%.3f\n", i, state.vout, state.il, duty);
        }
    }
    
    printf("\n仿真结果：\n");
    printf("最终输出电压: %.3f V\n", state.vout);
    printf("最终电感电流: %.3f A\n", state.il);
    printf("最终占空比: %.3f\n", duty);
    
    // 理论计算
    double theoretical_duty = fabs(VREF) / (VIN + fabs(VREF));
    double theoretical_il = fabs(VREF) / RLOAD * (1.0 / (1.0 - theoretical_duty));
    
    printf("\n理论值：\n");
    printf("理论占空比: %.3f\n", theoretical_duty);
    printf("理论电感电流: %.3f A\n", theoretical_il);
    
    // 误差分析
    printf("\n误差分析：\n");
    printf("电压误差: %.3f V (%.1f%%)\n", 
           state.vout - VREF, 
           fabs(state.vout - VREF)/fabs(VREF)*100);
    
    return 0;
}