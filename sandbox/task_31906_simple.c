#include <stdio.h>
#include <math.h>
#include <stdint.h>

#define MOCK_PWM_BASE 0x40000000
#define PERIOD_REG (*(volatile uint32_t*)(MOCK_PWM_BASE + 0x00))
#define DUTY1_REG (*(volatile uint32_t*)(MOCK_PWM_BASE + 0x04))
#define DUTY2_REG (*(volatile uint32_t*)(MOCK_PWM_BASE + 0x08))
#define DEADTIME_REG (*(volatile uint32_t*)(MOCK_PWM_BASE + 0x0C))

typedef struct {
    uint32_t PERIOD;
    uint32_t DUTY1;
    uint32_t DUTY2;
    uint32_t DEADTIME;
} PWM_Control;

double calculate_phase_shift(double Vin, double Vout, double fsw) {
    if (Vin == 0 || fsw == 0) return 0.0;
    double ratio = Vout / Vin;
    if (ratio > 1.0) ratio = 1.0;
    if (ratio < -1.0) ratio = -1.0;
    double angle_rad = asin(ratio);
    double angle_deg = angle_rad * (180.0 / 3.14159265358979323846);
    if (angle_deg < 0) angle_deg = 0;
    if (angle_deg > 180) angle_deg = 180;
    return angle_deg;
}

double optimize_deadtime(double phase_shift_deg, double fsw, int current_direction) {
    if (fsw == 0) return 0.1;
    double t_dead = 0.1 * (180.0 - phase_shift_deg) / fsw;
    if (t_dead < 0.1) t_dead = 0.1;
    return t_dead;
}

void configure_pwm_registers(PWM_Control* pwm, double fsw, double duty1, double duty2, double deadtime_us) {
    pwm->PERIOD = (uint32_t)(1e6 / fsw);
    pwm->DUTY1 = (uint32_t)(pwm->PERIOD * duty1 / 100.0);
    pwm->DUTY2 = (uint32_t)(pwm->PERIOD * duty2 / 100.0);
    pwm->DEADTIME = (uint32_t)(deadtime_us);
}

int main() {
    printf("Push-Pull Converter Phase Shift Control Algorithm Simulation\n");
    printf("===========================================================\n\n");
    
    double Vin, Vout, fsw;
    double phase_shift, deadtime;
    
    printf("Test 1: Vin=48V, Vout=12V, fsw=100kHz\n");
    Vin = 48.0;
    Vout = 12.0;
    fsw = 100000.0;
    phase_shift = calculate_phase_shift(Vin, Vout, fsw);
    deadtime = optimize_deadtime(phase_shift, fsw, 1);
    printf("  Phase shift angle: %.2f degrees\n", phase_shift);
    printf("  Optimized deadtime: %.2f us\n\n", deadtime);
    
    printf("Test 2: Vin=24V, Vout=5V, fsw=200kHz\n");
    Vin = 24.0;
    Vout = 5.0;
    fsw = 200000.0;
    phase_shift = calculate_phase_shift(Vin, Vout, fsw);
    deadtime = optimize_deadtime(phase_shift, fsw, 1);
    printf("  Phase shift angle: %.2f degrees\n", phase_shift);
    printf("  Optimized deadtime: %.2f us\n\n", deadtime);
    
    printf("Test 3: PWM Register Configuration Simulation\n");
    PWM_Control pwm_ctrl;
    configure_pwm_registers(&pwm_ctrl, 100000.0, 45.0, 45.0, 0.15);
    printf("  PERIOD register: %u\n", pwm_ctrl.PERIOD);
    printf("  DUTY1 register: %u\n", pwm_ctrl.DUTY1);
    printf("  DUTY2 register: %u\n", pwm_ctrl.DUTY2);
    printf("  DEADTIME register: %u\n", pwm_ctrl.DEADTIME);
    
    printf("\nHardware Register Macro Definition Test:\n");
    PERIOD_REG = 1000;
    DUTY1_REG = 450;
    DUTY2_REG = 450;
    DEADTIME_REG = 150;
    printf("  PERIOD_REG: %u\n", PERIOD_REG);
    printf("  DUTY1_REG: %u\n", DUTY1_REG);
    printf("  DUTY2_REG: %u\n", DUTY2_REG);
    printf("  DEADTIME_REG: %u\n", DEADTIME_REG);
    
    return 0;
}