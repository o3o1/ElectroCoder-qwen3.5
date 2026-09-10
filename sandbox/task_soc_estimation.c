#include <stdio.h>
#include <math.h>

double soc_ah_integration(double initial_soc, double capacity_ah, double current_a[], double time_s[], int n) {
    double soc = initial_soc;
    
    for (int i = 0; i < n; i++) {
        double delta_soc = (current_a[i] * time_s[i] / (capacity_ah * 3600.0)) * 100.0;
        soc += delta_soc;
        
        if (soc > 100.0) soc = 100.0;
        if (soc < 0.0) soc = 0.0;
    }
    
    return soc;
}

int main() {
    printf("=== 电池SoC安时积分法估算测试 ===\n\n");
    
    // 测试1：恒流放电场景
    printf("测试1：恒流放电场景\n");
    double initial_soc1 = 100.0;  // 初始SoC 100%
    double capacity1 = 50.0;      // 电池容量 50Ah
    
    // 恒流放电：10A放电1小时（3600秒）
    double current1[] = {10.0, 10.0, 10.0, 10.0, 10.0, 10.0};
    double time1[] = {600.0, 600.0, 600.0, 600.0, 600.0, 600.0};  // 每段10分钟
    int n1 = 6;
    
    printf("初始SoC: %.2f%%\n", initial_soc1);
    printf("电池容量: %.2f Ah\n", capacity1);
    printf("电流序列: ");
    for (int i = 0; i < n1; i++) printf("%.1fA ", current1[i]);
    printf("\n时间序列: ");
    for (int i = 0; i < n1; i++) printf("%.0fs ", time1[i]);
    
    double soc1 = soc_ah_integration(initial_soc1, capacity1, current1, time1, n1);
    printf("\n估算SoC: %.2f%%\n", soc1);
    
    // 理论计算：放电10A × 3600s = 10Ah，占50Ah的20%，所以剩余80%
    printf("理论值: 80.00%%\n\n");
    
    // 测试2：充放电交替场景
    printf("测试2：充放电交替场景\n");
    double initial_soc2 = 50.0;   // 初始SoC 50%
    double capacity2 = 30.0;      // 电池容量 30Ah
    
    // 充放电交替：放电5A 30分钟，充电-8A 20分钟，放电3A 40分钟
    double current2[] = {5.0, -8.0, 3.0};
    double time2[] = {1800.0, 1200.0, 2400.0};  // 30min, 20min, 40min
    int n2 = 3;
    
    printf("初始SoC: %.2f%%\n", initial_soc2);
    printf("电池容量: %.2f Ah\n", capacity2);
    printf("电流序列: ");
    for (int i = 0; i < n2; i++) printf("%.1fA ", current2[i]);
    printf("\n时间序列: ");
    for (int i = 0; i < n2; i++) printf("%.0fs ", time2[i]);
    
    double soc2 = soc_ah_integration(initial_soc2, capacity2, current2, time2, n2);
    printf("\n估算SoC: %.2f%%\n", soc2);
    
    // 理论计算：
    // 放电1: 5A × 1800s = 9000As = 2.5Ah
    // 充电: -8A × 1200s = -9600As = -2.667Ah  
    // 放电2: 3A × 2400s = 7200As = 2.0Ah
    // 总变化: 2.5 - 2.667 + 2.0 = 1.833Ah
    // SoC变化: (1.833/30)×100 = 6.11%
    // 最终SoC: 50 + 6.11 = 56.11%
    printf("理论值: 56.11%%\n\n");
    
    // 测试3：边界测试（SoC超限）
    printf("测试3：边界测试（过充保护）\n");
    double initial_soc3 = 95.0;
    double capacity3 = 20.0;
    double current3[] = {-10.0, -10.0};  // 大电流充电
    double time3[] = {1800.0, 1800.0};   // 每段30分钟
    int n3 = 2;
    
    double soc3 = soc_ah_integration(initial_soc3, capacity3, current3, time3, n3);
    printf("初始SoC: %.2f%%，充电后SoC: %.2f%% (应限制在100%%)\n", initial_soc3, soc3);
    
    // 测试4：边界测试（过放保护）
    printf("\n测试4：边界测试（过放保护）\n");
    double initial_soc4 = 5.0;
    double capacity4 = 20.0;
    double current4[] = {15.0, 15.0};    // 大电流放电
    double time4[] = {1800.0, 1800.0};   // 每段30分钟
    int n4 = 2;
    
    double soc4 = soc_ah_integration(initial_soc4, capacity4, current4, time4, n4);
    printf("初始SoC: %.2f%%，放电后SoC: %.2f%% (应限制在0%%)\n", initial_soc4, soc4);
    
    return 0;
}