#include <stdio.h>
#include <math.h>

// PR控制器结构体
typedef struct {
    double Kp;      // 比例增益
    double Kr;      // 谐振增益
    double wc;      // 截止频率 (rad/s)
    double w0;      // 谐振频率 (rad/s)
    
    // 离散化系数（双线性变换）
    double a0, a1, a2;
    double b0, b1, b2;
    
    // 状态变量
    double x1, x2;  // 前两个输入状态
    double y1, y2;  // 前两个输出状态
} PRController;

// 初始化PR控制器（使用双线性变换离散化）
void pr_init(PRController* ctrl, double Kp, double Kr, double fc, double f0, double fs) {
    ctrl->Kp = Kp;
    ctrl->Kr = Kr;
    ctrl->wc = 2.0 * 3.141592653589793 * fc;  // 截止角频率
    ctrl->w0 = 2.0 * 3.141592653589793 * f0;  // 谐振角频率
    
    double T = 1.0 / fs;  // 采样周期
    
    // 双线性变换：s = (2/T) * (1 - z^-1) / (1 + z^-1)
    // 连续传递函数：G(s) = Kp + (2*Kr*wc*s) / (s^2 + 2*wc*s + w0^2)
    
    // 离散化谐振部分
    double T2 = T * T;
    double wc2 = ctrl->wc * ctrl->wc;
    double w02 = ctrl->w0 * ctrl->w0;
    
    // 分母系数
    double den = 4.0 + 4.0 * ctrl->wc * T + w02 * T2;
    
    // 离散传递函数系数
    // H(z) = (b0 + b1*z^-1 + b2*z^-2) / (1 + a1*z^-1 + a2*z^-2)
    
    // 谐振部分系数
    double b0_res = 8.0 * ctrl->Kr * ctrl->wc * T / den;
    double b1_res = 0.0;
    double b2_res = -b0_res;
    
    double a1_res = (2.0 * w02 * T2 - 8.0) / den;
    double a2_res = (4.0 - 4.0 * ctrl->wc * T + w02 * T2) / den;
    
    // 加上比例部分
    ctrl->b0 = ctrl->Kp + b0_res;
    ctrl->b1 = ctrl->Kp * a1_res + b1_res;
    ctrl->b2 = ctrl->Kp * a2_res + b2_res;
    ctrl->a0 = 1.0;
    ctrl->a1 = a1_res;
    ctrl->a2 = a2_res;
    
    // 初始化状态变量
    ctrl->x1 = 0.0;
    ctrl->x2 = 0.0;
    ctrl->y1 = 0.0;
    ctrl->y2 = 0.0;
}

// PR控制器计算函数
double pr_calculate(PRController* ctrl, double input) {
    // 差分方程：y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2] - a1*y[n-1] - a2*y[n-2]
    double output = ctrl->b0 * input + ctrl->b1 * ctrl->x1 + ctrl->b2 * ctrl->x2
                   - ctrl->a1 * ctrl->y1 - ctrl->a2 * ctrl->y2;
    
    // 更新状态变量
    ctrl->x2 = ctrl->x1;
    ctrl->x1 = input;
    ctrl->y2 = ctrl->y1;
    ctrl->y1 = output;
    
    return output;
}

// 生成测试信号：50Hz基波 + 250Hz（5次谐波）
double generate_test_signal(double t, double fs) {
    double f1 = 50.0;    // 基波频率
    double f5 = 250.0;   // 5次谐波频率
    double amplitude1 = 1.0;
    double amplitude5 = 0.3;  // 谐波幅度较小
    
    return amplitude1 * sin(2.0 * 3.141592653589793 * f1 * t) + 
           amplitude5 * sin(2.0 * 3.141592653589793 * f5 * t);
}

int main() {
    // 控制器参数
    double Kp = 0.5;
    double Kr = 10.0;
    double fc = 5.0;     // 截止频率 5Hz
    double f0 = 50.0;    // 谐振频率 50Hz
    double fs = 1000.0;  // 采样频率 1kHz
    
    // 初始化PR控制器
    PRController ctrl;
    pr_init(&ctrl, Kp, Kr, fc, f0, fs);
    
    // 打印参数信息
    printf("PR控制器参数：\n");
    printf("Kp = %.2f, Kr = %.2f, fc = %.1f Hz, f0 = %.1f Hz\n", Kp, Kr, fc, f0);
    printf("采样频率 fs = %.1f Hz\n\n", fs);
    
    // 运行模拟
    int num_samples = 200;
    printf("前10个采样点的输入和输出：\n");
    printf("采样点\t时间(s)\t\t输入信号\t控制器输出\n");
    printf("--------------------------------------------------------\n");
    
    for (int i = 0; i < num_samples; i++) {
        double t = i / fs;  // 时间
        double input = generate_test_signal(t, fs);
        double output = pr_calculate(&ctrl, input);
        
        if (i < 10) {
            printf("%d\t%.4f\t\t%.6f\t%.6f\n", i, t, input, output);
        }
    }
    
    printf("\n模拟完成，共运行 %d 个采样点。\n", num_samples);
    
    return 0;
}