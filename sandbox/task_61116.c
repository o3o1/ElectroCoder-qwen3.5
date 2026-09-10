#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// 模拟开关管控制引脚宏定义
#define Q1_HIGH() printf("Q1导通 ")
#define Q1_LOW()  printf("Q1关断 ")
#define Q2_HIGH() printf("Q2导通 ")
#define Q2_LOW()  printf("Q2关断 ")
#define Q3_HIGH() printf("Q3导通 ")
#define Q3_LOW()  printf("Q3关断 ")
#define Q4_HIGH() printf("Q4导通 ")
#define Q4_LOW()  printf("Q4关断 ")

// 模拟硬件寄存器
#define PWM_PERIOD_US 100.0f  // PWM周期100μs
#define SWITCHING_FREQ 10000.0f  // 开关频率10kHz

// 移相控制函数
void phase_shift_control(float phase_shift_deg, float dead_time_us) {
    printf("\n=== 移相控制参数 ===\n");
    printf("移相角度: %.1f度\n", phase_shift_deg);
    printf("死区时间: %.1fμs\n", dead_time_us);
    printf("PWM周期: %.1fμs\n", PWM_PERIOD_US);
    
    // 角度转弧度
    float phase_shift_rad = phase_shift_deg * 3.1415926535f / 180.0f;
    
    // 计算时间参数（基于PWM周期）
    float phase_shift_time_us = (phase_shift_deg / 360.0f) * PWM_PERIOD_US;
    float half_period_us = PWM_PERIOD_US / 2.0f;
    
    printf("\n=== 时间参数计算 ===\n");
    printf("移相时间: %.2fμs\n", phase_shift_time_us);
    printf("半周期时间: %.2fμs\n", half_period_us);
    
    // 定义关键时间点
    float t0 = 0.0f;           // 周期开始
    float t1 = dead_time_us;   // 死区结束
    float t2 = phase_shift_time_us;  // 移相点
    float t3 = phase_shift_time_us + dead_time_us;  // 移相后死区结束
    float t4 = half_period_us;  // 半周期点
    float t5 = half_period_us + dead_time_us;  // 半周期后死区结束
    float t6 = half_period_us + phase_shift_time_us;  // 下半周期移相点
    float t7 = half_period_us + phase_shift_time_us + dead_time_us;  // 下半周期移相后死区结束
    float t8 = PWM_PERIOD_US;  // 周期结束
    
    printf("\n=== 开关管时序表 ===\n");
    printf("时间(μs) | Q1 Q2 Q3 Q4 | 状态说明\n");
    printf("---------|------------|----------------\n");
    
    // 时间点0：周期开始
    printf("%6.1f  | ", t0);
    Q1_HIGH(); Q2_LOW(); Q3_LOW(); Q4_HIGH();
    printf("| 初始状态：Q1,Q4导通\n");
    
    // 时间点1：死区结束，Q4关断，Q3准备导通（滞后桥臂ZVS条件）
    printf("%6.1f  | ", t1);
    Q1_HIGH(); Q2_LOW(); Q3_LOW(); Q4_LOW();
    printf("| 死区：Q4关断，等待ZVS\n");
    
    // 时间点2：移相点，Q3导通（滞后桥臂实现ZVS）
    printf("%6.1f  | ", t2);
    Q1_HIGH(); Q2_LOW(); Q3_HIGH(); Q4_LOW();
    printf("| 移相：Q3导通（ZVS）\n");
    
    // 时间点3：移相后死区结束
    printf("%6.1f  | ", t3);
    Q1_HIGH(); Q2_LOW(); Q3_HIGH(); Q4_LOW();
    printf("| 稳定：Q1,Q3导通\n");
    
    // 时间点4：半周期点，Q1关断
    printf("%6.1f  | ", t4);
    Q1_LOW(); Q2_LOW(); Q3_HIGH(); Q4_LOW();
    printf("| 半周期：Q1关断，死区开始\n");
    
    // 时间点5：半周期后死区结束，Q2导通
    printf("%6.1f  | ", t5);
    Q1_LOW(); Q2_HIGH(); Q3_HIGH(); Q4_LOW();
    printf("| Q2导通\n");
    
    // 时间点6：下半周期移相点，Q3关断
    printf("%6.1f  | ", t6);
    Q1_LOW(); Q2_HIGH(); Q3_LOW(); Q4_LOW();
    printf("| 移相：Q3关断，等待ZVS\n");
    
    // 时间点7：下半周期移相后死区结束，Q4导通（滞后桥臂ZVS）
    printf("%6.1f  | ", t7);
    Q1_LOW(); Q2_HIGH(); Q3_LOW(); Q4_HIGH();
    printf("| Q4导通（ZVS）\n");
    
    // 时间点8：周期结束，回到初始状态
    printf("%6.1f  | ", t8);
    Q1_HIGH(); Q2_LOW(); Q3_LOW(); Q4_HIGH();
    printf("| 周期结束，回到初始\n");
    
    printf("\n=== ZVS实现逻辑说明 ===\n");
    printf("1. 滞后桥臂(Q3,Q4)在电压过零时导通实现ZVS\n");
    printf("2. 移相角度%.1f度对应移相时间%.2fμs\n", phase_shift_deg, phase_shift_time_us);
    printf("3. 死区时间%.1fμs确保开关管不会同时导通\n", dead_time_us);
    printf("4. Q3在t=%.1fμs导通时，谐振电流已使Q3两端电压为零\n", t2);
    printf("5. Q4在t=%.1fμs导通时，谐振电流已使Q4两端电压为零\n", t7);
}

int main() {
    printf("全桥变换器移相控制ZVS算法模拟\n");
    printf("================================\n");
    
    // 测试情况1：移相角30度，死区时间2μs
    printf("\n测试情况1：");
    phase_shift_control(30.0f, 2.0f);
    
    // 测试情况2：移相角60度，死区时间1.5μs
    printf("\n\n测试情况2：");
    phase_shift_control(60.0f, 1.5f);
    
    // 测试情况3：移相角0度，死区时间3μs
    printf("\n\n测试情况3：");
    phase_shift_control(0.0f, 3.0f);
    
    printf("\n\n模拟完成！\n");
    return 0;
}