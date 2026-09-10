#include <stdio.h>
#include <math.h>
#include <stdint.h>

// 全局变量用于保存上一次的Δp值用于数值积分
static double last_delta_p = 0.0;
static double last_time = 0.0;
static int first_run = 1;

double virtual_capacitor_control(double t, double P_ac, double f, double C_v, double k) {
    // 计算电网角频率
    double omega = 2.0 * M_PI * f;
    
    // 计算瞬时功率脉动分量 Δp = P_ac * cos(2ωt)
    double delta_p = P_ac * cos(2.0 * omega * t);
    
    // 计算积分项：v_c = (1/C_v) * ∫Δp dt
    double v_c = 0.0;
    
    if (first_run) {
        // 第一次运行，无法进行数值积分，返回0
        first_run = 0;
        last_delta_p = delta_p;
        last_time = t;
        v_c = 0.0;
    } else {
        // 使用梯形法进行数值积分
        double dt = t - last_time;
        if (dt > 0) {
            // 梯形法：∫f(t)dt ≈ (f(t1) + f(t2))/2 * dt
            double integral = (last_delta_p + delta_p) / 2.0 * dt;
            
            // 累加积分值
            static double total_integral = 0.0;
            total_integral += integral;
            
            // 计算虚拟电容电压
            v_c = total_integral / C_v;
        }
        
        // 更新上一次的值
        last_delta_p = delta_p;
        last_time = t;
    }
    
    // 计算补偿电压
    double v_comp = k * v_c;
    
    return v_comp;
}

// 重置函数，用于测试多个独立测试用例
void reset_virtual_capacitor() {
    last_delta_p = 0.0;
    last_time = 0.0;
    first_run = 1;
}

int main() {
    printf("光伏并网逆变器虚拟电容法功率解耦控制测试\n");
    printf("=========================================\n\n");
    
    // 测试1：计算t=0.01s时的补偿电压
    printf("测试1：单点计算\n");
    printf("参数：f=50Hz, P_ac=100W, C_v=0.001F, k=0.5, t=0.01s\n");
    
    reset_virtual_capacitor();
    
    // 先计算一个初始点，确保积分正确
    virtual_capacitor_control(0.0, 100.0, 50.0, 0.001, 0.5);
    
    // 计算目标点
    double v_comp1 = virtual_capacitor_control(0.01, 100.0, 50.0, 0.001, 0.5);
    printf("补偿电压 v_comp = %.6f V\n\n", v_comp1);
    
    // 测试2：连续计算5个点
    printf("测试2：连续计算5个点（t=0~0.02s，步长0.005s）\n");
    printf("参数：f=50Hz, P_ac=100W, C_v=0.001F, k=0.5\n");
    printf("时间(s)    补偿电压(V)\n");
    printf("----------------------\n");
    
    reset_virtual_capacitor();
    
    // 计算5个时间点
    double time_points[] = {0.0, 0.005, 0.01, 0.015, 0.02};
    int num_points = sizeof(time_points) / sizeof(time_points[0]);
    
    for (int i = 0; i < num_points; i++) {
        double t = time_points[i];
        double v_comp = virtual_capacitor_control(t, 100.0, 50.0, 0.001, 0.5);
        printf("%.6f   %.6f\n", t, v_comp);
    }
    
    printf("\n测试完成！\n");
    
    return 0;
}