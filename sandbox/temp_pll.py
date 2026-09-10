import math 
PI = 3.14159265358979323846 
TWO_PI = 2.0 * PI 
 
class PLL: 
    def __init__(self): 
        self.theta = 0.0 
        self.f_est = 50.0 
        self.filter_state = 0.0 
        self.prev_signal = 0.0 
        self.zero_cross_detected = 0 
 
    def low_pass_filter(self, input_val, alpha): 
        self.filter_state = alpha * input_val + (1.0 - alpha) * self.filter_state 
        return self.filter_state 
 
    def detect_zero_crossing(self, current_signal): 
        return (self.prev_signal <= 0.0 and current_signal > 0.0) 
 
    def update(self, signal, dt, kp, ki): 
        self.zero_cross_detected = self.detect_zero_crossing(signal) 
        self.prev_signal = signal 
 
        if self.zero_cross_detected: 
            phase_error = -self.theta 
            while phase_error > PI: phase_error -= TWO_PI 
            while phase_error < -PI: phase_error += TWO_PI 
 
            alpha = 0.05 
            filtered_error = self.low_pass_filter(phase_error, alpha) 
 
            self.f_est += ki * filtered_error 
 
            if self.f_est < 45.0: self.f_est = 45.0 
            if self.f_est > 55.0: self.f_est = 55.0 
 
        self.theta += TWO_PI * self.f_est * dt 
 
        if self.theta >= TWO_PI: self.theta -= TWO_PI 
        if self.theta < 0.0: self.theta += TWO_PI 
 
def generate_signal(t, Vm, f, phi): 
    return Vm * math.sin(TWO_PI * f * t + phi) 
 
Vm = 1.0 
f_input = 50.0 
phi = 0.1 
fs = 2000.0 
dt = 1.0 / fs 
 
num_samples = 1000 
print_interval = 100 
 
pll = PLL() 
kp = 0.0 
ki = 0.1 
 
print("单相锁相环(PLL)仿真测试") 
print() 
print("-" * 60) 
 
steady_state_error_sum = 0.0 
steady_state_count = 0 
 
for i in range(num_samples): 
    t = i * dt 
    signal = generate_signal(t, Vm, f_input, phi) 
    pll.update(signal, dt, kp, ki) 
 
    if i % print_interval == 0: 
 
    if i >= 500: 
        steady_state_error_sum += abs(pll.f_est - f_input) 
        steady_state_count += 1 
 
steady_state_error = steady_state_error_sum / steady_state_count 
 
print() 
print("-" * 60) 
print("锁相环收敛结果:") 
