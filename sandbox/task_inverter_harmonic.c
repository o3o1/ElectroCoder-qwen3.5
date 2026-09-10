#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define PI 3.14159265358979323846
#define FS 10000.0  // 采样频率 10kHz
#define F1 50.0     // 基波频率 50Hz
#define F3 150.0    // 3次谐波频率 150Hz
#define F5 250.0    // 5次谐波频率 250Hz
#define N 1024      // 采样点数

// DFT结构体，存储谐波信息
typedef struct {
    double amplitude;  // 幅值
    double phase;      // 相位（弧度）
} HarmonicInfo;

// 计算单个频率分量的DFT
HarmonicInfo compute_dft_component(double *signal, int n, double freq) {
    HarmonicInfo result = {0.0, 0.0};
    double real = 0.0;
    double imag = 0.0;
    
    for (int k = 0; k < n; k++) {
        double angle = 2.0 * PI * freq * k / FS;
        real += signal[k] * cos(angle);
        imag += signal[k] * sin(angle);
    }
    
    real = 2.0 * real / n;
    imag = 2.0 * imag / n;
    
    result.amplitude = sqrt(real * real + imag * imag);
    result.phase = atan2(imag, real);
    
    return result;
}

// 谐波补偿函数
void harmonic_compensation(double *voltage, int n, double *compensated) {
    // 检测基波、3次谐波和5次谐波
    HarmonicInfo fundamental = compute_dft_component(voltage, n, F1);
    HarmonicInfo harmonic3 = compute_dft_component(voltage, n, F3);
    HarmonicInfo harmonic5 = compute_dft_component(voltage, n, F5);
    
    // 从原始信号中减去3次和5次谐波分量
    for (int i = 0; i < n; i++) {
        double t = i / FS;
        
        // 3次谐波分量
        double h3 = harmonic3.amplitude * cos(2.0 * PI * F3 * t + harmonic3.phase);
        
        // 5次谐波分量
        double h5 = harmonic5.amplitude * cos(2.0 * PI * F5 * t + harmonic5.phase);
        
        // 补偿后信号 = 原始信号 - 3次谐波 - 5次谐波
        compensated[i] = voltage[i] - h3 - h5;
    }
}

int main() {
    double voltage[N];
    double compensated[N];
    
    // 生成测试信号参数
    double A1 = 311.0;    // 基波幅值 311V (对应220V有效值)
    double A3 = 15.5;     // 3次谐波幅值 15.5V (5%畸变)
    double A5 = 9.3;      // 5次谐波幅值 9.3V (3%畸变)
    
    double phi1 = 0.0;    // 基波初始相位
    double phi3 = PI/6;   // 3次谐波初始相位 30度
    double phi5 = PI/4;   // 5次谐波初始相位 45度
    
    // 生成测试信号
    for (int i = 0; i < N; i++) {
        double t = i / FS;
        
        // 基波分量
        double v1 = A1 * cos(2.0 * PI * F1 * t + phi1);
        
        // 3次谐波分量
        double v3 = A3 * cos(2.0 * PI * F3 * t + phi3);
        
        // 5次谐波分量
        double v5 = A5 * cos(2.0 * PI * F5 * t + phi5);
        
        voltage[i] = v1 + v3 + v5;
    }
    
    // 调用谐波补偿函数
    harmonic_compensation(voltage, N, compensated);
    
    // 打印前10个采样点的结果
    printf("逆变器输出电压谐波补偿结果：\n");
    printf("采样频率：%.0f Hz，采样点数：%d\n", FS, N);
    printf("基波频率：%.0f Hz，3次谐波：%.0f Hz，5次谐波：%.0f Hz\n\n", F1, F3, F5);
    
    for (int i = 0; i < 10; i++) {
        printf("Sample [%d]: raw=%7.2f V, compensated=%7.2f V\n", 
               i, voltage[i], compensated[i]);
    }
    
    // 计算并显示谐波检测结果
    printf("\n谐波检测结果：\n");
    
    HarmonicInfo fundamental = compute_dft_component(voltage, N, F1);
    HarmonicInfo harmonic3 = compute_dft_component(voltage, N, F3);
    HarmonicInfo harmonic5 = compute_dft_component(voltage, N, F5);
    
    printf("基波(50Hz): 幅值=%7.2f V, 相位=%7.2f 度\n", 
           fundamental.amplitude, fundamental.phase * 180.0 / PI);
    printf("3次谐波(150Hz): 幅值=%7.2f V, 相位=%7.2f 度\n", 
           harmonic3.amplitude, harmonic3.phase * 180.0 / PI);
    printf("5次谐波(250Hz): 幅值=%7.2f V, 相位=%7.2f 度\n", 
           harmonic5.amplitude, harmonic5.phase * 180.0 / PI);
    
    // 计算THD（总谐波畸变率）
    double thd = sqrt(harmonic3.amplitude * harmonic3.amplitude + 
                      harmonic5.amplitude * harmonic5.amplitude) / 
                 fundamental.amplitude * 100.0;
    printf("\n总谐波畸变率(THD): %.2f%%\n", thd);
    
    return 0;
}