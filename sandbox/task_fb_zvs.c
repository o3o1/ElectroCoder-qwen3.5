#include <stdio.h>
#include <math.h>

#define MOCK_PWM_CMP_BASE 0x40000000
#define MOCK_DEAD_TIME_BASE 0x40000004
#define MOCK_PHASE_SHIFT_BASE 0x40000008

typedef struct {
    volatile unsigned int CMPA;
    volatile unsigned int CMPB;
} PWM_CMP_TypeDef;

typedef struct {
    volatile unsigned int DEAD_A;
    volatile unsigned int DEAD_B;
} DEAD_TIME_TypeDef;

typedef struct {
    volatile unsigned int PHASE_AB;
    volatile unsigned int PHASE_BA;
} PHASE_SHIFT_TypeDef;

#define PWM_CMP ((PWM_CMP_TypeDef *)MOCK_PWM_CMP_BASE)
#define DEAD_TIME ((DEAD_TIME_TypeDef *)MOCK_DEAD_TIME_BASE)
#define PHASE_SHIFT ((PHASE_SHIFT_TypeDef *)MOCK_PHASE_SHIFT_BASE)

void calculate_zvs_timing(double f_sw, double Lr, double Cr, double I_load, 
                         double *t_dead_zvs, double *phi_zvs) {
    double resonant_period = M_PI * sqrt(Lr * Cr);
    double t_min = resonant_period / 2.0;
    
    double load_factor = 1.0 / (1.0 + 0.1 * I_load);
    *t_dead_zvs = t_min * load_factor;
    
    if (*t_dead_zvs < t_min) {
        *t_dead_zvs = t_min;
    }
    
    *phi_zvs = 2.0 * M_PI * f_sw * (*t_dead_zvs);
}

void mock_registers(double t_dead_ns, double phi_rad, unsigned int pwm_period) {
    unsigned int dead_time_ticks = (unsigned int)(t_dead_ns * 1e-9 * 100e6);
    unsigned int phase_shift_ticks = (unsigned int)(phi_rad / (2 * M_PI) * pwm_period);
    
    DEAD_TIME->DEAD_A = dead_time_ticks;
    DEAD_TIME->DEAD_B = dead_time_ticks;
    PHASE_SHIFT->PHASE_AB = phase_shift_ticks;
    PHASE_SHIFT->PHASE_BA = phase_shift_ticks;
}

int main() {
    printf("全桥变换器移相控制ZVS算法模拟\n");
    printf("================================\n\n");
    
    double t_dead_zvs1, phi_zvs1;
    double t_dead_zvs2, phi_zvs2;
    
    double f_sw1 = 100000.0;
    double Lr1 = 10e-6;
    double Cr1 = 1e-9;
    double I_load1 = 5.0;
    
    double f_sw2 = 50000.0;
    double Lr2 = 15e-6;
    double Cr2 = 2e-9;
    double I_load2 = 2.5;
    
    calculate_zvs_timing(f_sw1, Lr1, Cr1, I_load1, &t_dead_zvs1, &phi_zvs1);
    calculate_zvs_timing(f_sw2, Lr2, Cr2, I_load2, &t_dead_zvs2, &phi_zvs2);
    
    printf("测试用例1:\n");
    printf("输入参数: f_sw=%.0f Hz, Lr=%.2e H, Cr=%.2e F, I_load=%.1f A\n", 
           f_sw1, Lr1, Cr1, I_load1);
    printf("计算结果: t_dead_zvs=%.2f ns, φ_zvs=%.4f rad (%.2f度)\n", 
           t_dead_zvs1 * 1e9, phi_zvs1, phi_zvs1 * 180 / M_PI);
    
    printf("\n测试用例2:\n");
    printf("输入参数: f_sw=%.0f Hz, Lr=%.2e H, Cr=%.2e F, I_load=%.1f A\n", 
           f_sw2, Lr2, Cr2, I_load2);
    printf("计算结果: t_dead_zvs=%.2f ns, φ_zvs=%.4f rad (%.2f度)\n", 
           t_dead_zvs2 * 1e9, phi_zvs2, phi_zvs2 * 180 / M_PI);
    
    unsigned int pwm_period = 1000;
    mock_registers(t_dead_zvs1, phi_zvs1, pwm_period);
    
    printf("\n模拟寄存器设置值:\n");
    printf("DEAD_TIME寄存器: DEAD_A=%u, DEAD_B=%u\n", 
           DEAD_TIME->DEAD_A, DEAD_TIME->DEAD_B);
    printf("PHASE_SHIFT寄存器: PHASE_AB=%u, PHASE_BA=%u\n", 
           PHASE_SHIFT->PHASE_AB, PHASE_SHIFT->PHASE_BA);
    
    return 0;
}