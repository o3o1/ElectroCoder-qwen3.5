#include <stdio.h>
#include <math.h>

#define PI 3.14159265358979323846
#define TWOPI (2.0 * PI)

int main() {
    double U_m = 311.0;
    double Ts = 0.0001;
    double omega_nom = 314.1592653589793;
    double Kp = 100.0;
    double Ki = 5000.0;
    double freq_jump_time = 0.1;
    
    double integral = 0.0;
    double omega_est = omega_nom;
    double theta_est = 0.0;
    double u_q_ref = 0.0;
    
    printf("三相锁相环(SRF-PLL)算法测试\n");
    printf("采样周期: %.6f s\n", Ts);
    printf("额定频率: %.2f Hz (ω=%.2f rad/s)\n", omega_nom/(2*PI), omega_nom);
    printf("PI参数: Kp=%.1f, Ki=%.1f\n", Kp, Ki);
    printf("\n");
    
    int total_samples = 2000;
    int print_before_jump = 10;
    int print_after_jump = 10;
    
    printf("前5个采样点:\n");
    printf("采样点 实际θ(rad) 估计θ(rad) 误差(rad)\n");
    
    for (int i = 0; i < total_samples; i++) {
        double t = i * Ts;
        double omega_actual;
        
        if (t < freq_jump_time) {
            omega_actual = omega_nom;
        } else {
            omega_actual = omega_nom * 1.01;
        }
        
        double theta_actual = omega_actual * t;
        while (theta_actual >= TWOPI) theta_actual -= TWOPI;
        while (theta_actual < 0) theta_actual += TWOPI;
        
        double u_a = U_m * sin(theta_actual);
        double u_b = U_m * sin(theta_actual - 2.0*PI/3.0);
        double u_c = U_m * sin(theta_actual + 2.0*PI/3.0);
        
        double k = 2.0 / 3.0;
        double u_alpha = k * (u_a - 0.5 * u_b - 0.5 * u_c);
        double u_beta = k * (sqrt(3.0) / 2.0 * u_b - sqrt(3.0) / 2.0 * u_c);
        
        double cos_theta = cos(theta_est);
        double sin_theta = sin(theta_est);
        double u_d = u_alpha * cos_theta + u_beta * sin_theta;
        double u_q = -u_alpha * sin_theta + u_beta * cos_theta;
        
        double error = u_q_ref - u_q;
        integral += error * Ts;
        double delta_omega = Kp * error + Ki * integral;
        omega_est = omega_nom + delta_omega;
        theta_est += omega_est * Ts;
        while (theta_est >= TWOPI) theta_est -= TWOPI;
        while (theta_est < 0) theta_est += TWOPI;
        
        double phase_error = theta_actual - theta_est;
        while (phase_error > PI) phase_error -= TWOPI;
        while (phase_error < -PI) phase_error += TWOPI;
        
        if (i < 5) {
            printf("%6d %12.6f %12.6f %12.6f\n", i, theta_actual, theta_est, phase_error);
        }
        
        if (t >= freq_jump_time - print_before_jump*Ts && t <= freq_jump_time + print_after_jump*Ts) {
            if (i == (int)(freq_jump_time/Ts) - print_before_jump) {
                printf("\n频率跳变前后各%d个采样点 (跳变时刻: %.3f s):\n", print_before_jump, freq_jump_time);
                printf("采样点 时间(s) 实际θ(rad) 估计θ(rad) 误差(rad)\n");
            }
            if (t >= freq_jump_time - print_before_jump*Ts && t <= freq_jump_time + print_after_jump*Ts) {
                printf("%6d %9.4f %12.6f %12.6f %12.6f\n", i, t, theta_actual, theta_est, phase_error);
            }
        }
    }
    
    printf("\n测试完成。\n");
    return 0;
}