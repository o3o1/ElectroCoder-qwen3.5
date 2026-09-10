#include <stdio.h>
#include <math.h>

#define PI 3.14159265358979323846

void spwm_segmented_sync_modulation(double theta, double m, int *sector, int *N, double *duty_cycle) {
    // 确保角度在0-2π范围内
    double normalized_theta = fmod(theta, 2*PI);
    if (normalized_theta < 0) normalized_theta += 2*PI;
    
    // 扇区判断
    if (normalized_theta < PI/3) {
        *sector = 1;
        *N = 24;
    } else if (normalized_theta < 2*PI/3) {
        *sector = 2;
        *N = 18;
    } else if (normalized_theta < PI) {
        *sector = 3;
        *N = 12;
    } else if (normalized_theta < 4*PI/3) {
        *sector = 4;
        *N = 12;
    } else if (normalized_theta < 5*PI/3) {
        *sector = 5;
        *N = 18;
    } else {
        *sector = 6;
        *N = 24;
    }
    
    // 占空比计算：duty_cycle = m * sin(θ)（取绝对值）
    *duty_cycle = fabs(m * sin(normalized_theta));
}

int main() {
    // 测试用例1
    double theta1 = 0.5;
    double m1 = 0.8;
    int sector1, N1;
    double duty1;
    
    spwm_segmented_sync_modulation(theta1, m1, &sector1, &N1, &duty1);
    printf("测试1：θ=%.3f rad, m=%.3f => 扇区=%d, N=%d, duty=%.3f\n", 
           theta1, m1, sector1, N1, duty1);
    
    // 测试用例2
    double theta2 = 2.5;
    double m2 = 0.6;
    int sector2, N2;
    double duty2;
    
    spwm_segmented_sync_modulation(theta2, m2, &sector2, &N2, &duty2);
    printf("测试2：θ=%.3f rad, m=%.3f => 扇区=%d, N=%d, duty=%.3f\n", 
           theta2, m2, sector2, N2, duty2);
    
    // 测试用例3
    double theta3 = 4.2;
    double m3 = 0.9;
    int sector3, N3;
    double duty3;
    
    spwm_segmented_sync_modulation(theta3, m3, &sector3, &N3, &duty3);
    printf("测试3：θ=%.3f rad, m=%.3f => 扇区=%d, N=%d, duty=%.3f\n", 
           theta3, m3, sector3, N3, duty3);
    
    return 0;
}