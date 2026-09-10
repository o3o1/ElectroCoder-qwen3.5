#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define PI 3.14159265358979323846
#define SAMPLE_RATE 10000.0f    // 10kHz采样率
#define GRID_FREQ 50.0f        // 电网频率50Hz
#define GRID_VOLTAGE 220.0f    // 电网电压幅值220V
#define SAMPLE_PERIOD (1.0f/SAMPLE_RATE)
#define OMEGA_NOMINAL (2.0f*PI*GRID_FREQ)  // 额定角频率

// 无锁相环同步控制结构体
typedef struct {
    float theta;        // 估算的相位角（弧度）
    float freq;         // 估算的频率（Hz）
    float u_alpha_prev; // 上一周期α轴电压
    float u_beta_prev;  // 上一周期β轴电压
    float integral;     // 积分器状态
    float kp;           // 比例系数
    float ki;           // 积分系数
} PLLessSync;

// Clarke变换：三相abc转αβ坐标系
void clarke_transform(float ua, float ub, float uc, float *u_alpha, float *u_beta) {
    // Clarke变换矩阵
    *u_alpha = (2.0f/3.0f) * (ua - 0.5f*ub - 0.5f*uc);
    *u_beta = (2.0f/3.0f) * (0.86602540378f*ub - 0.86602540378f*uc); // sqrt(3)/2 ≈ 0.8660254
}

// 无锁相环同步控制核心算法
void plless_sync_update(PLLessSync *sync, float ua, float ub, float uc) {
    float u_alpha, u_beta;
    
    // 1. Clarke变换
    clarke_transform(ua, ub, uc, &u_alpha, &u_beta);
    
    // 2. 计算瞬时无功功率 q = uα * uβ' - uβ * uα'
    float q = u_alpha * sync->u_beta_prev - u_beta * sync->u_alpha_prev;
    
    // 3. 计算电压幅值平方（用于归一化）
    float u_sq = u_alpha*u_alpha + u_beta*u_beta;
    
    // 4. 避免除零错误，计算相位误差
    float phase_error = 0.0f;
    if (u_sq > 1e-6f) {
        // 相位误差 ≈ q / u_sq（小角度近似）
        phase_error = q / u_sq;
    }
    
    // 5. PI控制器处理相位误差
    float delta_theta = sync->kp * phase_error + sync->ki * sync->integral;
    
    // 6. 更新积分器状态
    sync->integral += phase_error * SAMPLE_PERIOD;
    
    // 7. 更新相位角（积分得到相位）
    sync->theta += (OMEGA_NOMINAL + delta_theta) * SAMPLE_PERIOD;
    
    // 8. 相位角归一化到 [-π, π] 范围
    while (sync->theta > PI) sync->theta -= 2.0f*PI;
    while (sync->theta < -PI) sync->theta += 2.0f*PI;
    
    // 9. 计算频率（相位变化率）
    sync->freq = GRID_FREQ + delta_theta / (2.0f*PI);
    
    // 10. 保存当前值供下一周期使用
    sync->u_alpha_prev = u_alpha;
    sync->u_beta_prev = u_beta;
}

// 初始化无锁相环同步控制器
void plless_sync_init(PLLessSync *sync) {
    sync->theta = 0.0f;
    sync->freq = GRID_FREQ;
    sync->u_alpha_prev = 0.0f;
    sync->u_beta_prev = 0.0f;
    sync->integral = 0.0f;
    sync->kp = 100.0f;  // 比例系数（需要根据系统调整）
    sync->ki = 5000.0f; // 积分系数（需要根据系统调整）
}

// 生成三相电压采样值
void generate_three_phase_voltage(float time, float *ua, float *ub, float *uc) {
    float omega = 2.0f * PI * GRID_FREQ;
    float phase = omega * time;
    
    *ua = GRID_VOLTAGE * sinf(phase);
    *ub = GRID_VOLTAGE * sinf(phase - 2.0f*PI/3.0f);
    *uc = GRID_VOLTAGE * sinf(phase + 2.0f*PI/3.0f);
}

int main() {
    PLLessSync sync;
    plless_sync_init(&sync);
    
    int total_samples = (int)(5.0f / GRID_FREQ * SAMPLE_RATE); // 5个周期的采样点数
    float time = 0.0f;
    
    float freq_sum = 0.0f;
    int valid_count = 0;
    
    printf("无锁相环同步控制算法仿真\n");
    printf("采样率: %.0f Hz, 电网频率: %.1f Hz, 电压幅值: %.1f V\n", 
           SAMPLE_RATE, GRID_FREQ, GRID_VOLTAGE);
    printf("============================================================\n");
    printf("采样点\t时间(s)\t相位角(rad)\t频率(Hz)\t频率误差(Hz)\n");
    printf("============================================================\n");
    
    for (int i = 0; i < total_samples; i++) {
        float ua, ub, uc;
        
        // 生成三相电压
        generate_three_phase_voltage(time, &ua, &ub, &uc);
        
        // 更新同步控制算法
        plless_sync_update(&sync, ua, ub, uc);
        
        // 计算频率误差
        float freq_error = sync.freq - GRID_FREQ;
        
        // 跳过前几个采样点（让算法稳定）
        if (i > 100) {
            freq_sum += fabsf(freq_error);
            valid_count++;
        }
        
        // 每100个采样点打印一次结果
        if (i % 100 == 0) {
            printf("%d\t%.4f\t%.4f\t\t%.4f\t\t%.4f\n", 
                   i, time, sync.theta, sync.freq, freq_error);
        }
        
        time += SAMPLE_PERIOD;
    }
    
    // 计算平均频率误差
    float avg_freq_error = (valid_count > 0) ? freq_sum / valid_count : 0.0f;
    
    printf("============================================================\n");
    printf("仿真完成！处理了 %d 个采样点（%.1f 个周期）\n", 
           total_samples, (float)total_samples * GRID_FREQ / SAMPLE_RATE);
    printf("平均频率估算误差: %.6f Hz\n", avg_freq_error);
    printf("相对误差: %.4f%%\n", avg_freq_error / GRID_FREQ * 100.0f);
    
    return 0;
}
