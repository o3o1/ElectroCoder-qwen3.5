import math
import numpy as np

PI = math.pi
TWOPI = 2.0 * PI

class ClarkTransform:
    @staticmethod
    def transform(u_a, u_b, u_c):
        k = 2.0 / 3.0
        u_alpha = k * (u_a - 0.5 * u_b - 0.5 * u_c)
        u_beta = k * (math.sqrt(3.0) / 2.0 * u_b - math.sqrt(3.0) / 2.0 * u_c)
        return u_alpha, u_beta

class ParkTransform:
    @staticmethod
    def transform(u_alpha, u_beta, theta_est):
        cos_theta = math.cos(theta_est)
        sin_theta = math.sin(theta_est)
        u_d = u_alpha * cos_theta + u_beta * sin_theta
        u_q = -u_alpha * sin_theta + u_beta * cos_theta
        return u_d, u_q

class PLLController:
    def __init__(self, Kp, Ki, Ts, omega_nom):
        self.Kp = Kp
        self.Ki = Ki
        self.Ts = Ts
        self.omega_nom = omega_nom
        self.u_q_ref = 0.0
        self.integral = 0.0
        self.omega_est = omega_nom
        self.theta_est = 0.0
    
    def update(self, u_q):
        error = self.u_q_ref - u_q
        self.integral += error * self.Ts
        delta_omega = self.Kp * error + self.Ki * self.integral
        self.omega_est = self.omega_nom + delta_omega
        self.theta_est += self.omega_est * self.Ts
        self.theta_est = self.normalize_angle(self.theta_est)
    
    @staticmethod
    def normalize_angle(angle):
        while angle >= TWOPI:
            angle -= TWOPI
        while angle < 0:
            angle += TWOPI
        return angle

def normalize_angle(angle):
    while angle >= TWOPI:
        angle -= TWOPI
    while angle < 0:
        angle += TWOPI
    return angle

def main():
    U_m = 311.0
    Ts = 0.0001
    omega_nom = 314.1592653589793
    Kp = 100.0
    Ki = 5000.0
    freq_jump_time = 0.1
    
    pll = PLLController(Kp, Ki, Ts, omega_nom)
    
    print("三相锁相环(SRF-PLL)算法测试")
    print(f"采样周期: {Ts:.6f} s")
    print(f"额定频率: {omega_nom/(2*PI):.2f} Hz (ω={omega_nom:.2f} rad/s)")
    print(f"PI参数: Kp={Kp:.1f}, Ki={Ki:.1f}")
    print()
    
    total_samples = 2000
    print_before_jump = 10
    print_after_jump = 10
    
    print("前5个采样点:")
    print("采样点 实际θ(rad) 估计θ(rad) 误差(rad)")
    
    for i in range(total_samples):
        t = i * Ts
        
        if t < freq_jump_time:
            omega_actual = omega_nom
        else:
            omega_actual = omega_nom * 1.01
        
        theta_actual = omega_actual * t
        theta_actual = normalize_angle(theta_actual)
        
        u_a = U_m * math.sin(theta_actual)
        u_b = U_m * math.sin(theta_actual - 2.0*PI/3.0)
        u_c = U_m * math.sin(theta_actual + 2.0*PI/3.0)
        
        u_alpha, u_beta = ClarkTransform.transform(u_a, u_b, u_c)
        u_d, u_q = ParkTransform.transform(u_alpha, u_beta, pll.theta_est)
        pll.update(u_q)
        
        phase_error = theta_actual - pll.theta_est
        while phase_error > PI:
            phase_error -= TWOPI
        while phase_error < -PI:
            phase_error += TWOPI
        
        if i < 5:
            print(f"{i:6d} {theta_actual:12.6f} {pll.theta_est:12.6f} {phase_error:12.6f}")
        
        if t >= freq_jump_time - print_before_jump*Ts and t <= freq_jump_time + print_after_jump*Ts:
            if i == int(freq_jump_time/Ts) - print_before_jump:
                print(f"\n频率跳变前后各{print_before_jump}个采样点 (跳变时刻: {freq_jump_time:.3f} s):")
                print("采样点 时间(s) 实际θ(rad) 估计θ(rad) 误差(rad)")
            if t >= freq_jump_time - print_before_jump*Ts and t <= freq_jump_time + print_after_jump*Ts:
                print(f"{i:6d} {t:9.4f} {theta_actual:12.6f} {pll.theta_est:12.6f} {phase_error:12.6f}")
    
    print("\n测试完成。")

if __name__ == "__main__":
    main()