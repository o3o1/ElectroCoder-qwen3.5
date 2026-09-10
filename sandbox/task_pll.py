import math

PI = 3.14159265358979323846
Ts = 0.0001
w0 = 2.0 * PI * 50.0
k = 1.414
Kp = 100.0
Ki = 1000.0

class SOGI_State:
    def __init__(self):
        self.v_alpha = 0.0
        self.v_beta = 0.0
        self.v_alpha_z1 = 0.0
        self.v_beta_z1 = 0.0

class PI_State:
    def __init__(self):
        self.integrator = 0.0
        self.output = 0.0

def SOGI_update(v_input, state):
    v_alpha_new = state.v_alpha_z1 + k * w0 * Ts * (v_input - state.v_alpha_z1) - w0 * Ts * state.v_beta_z1
    v_beta_new = state.v_beta_z1 + w0 * Ts * v_alpha_new
    
    state.v_alpha = v_alpha_new
    state.v_beta = v_beta_new
    state.v_alpha_z1 = v_alpha_new
    state.v_beta_z1 = v_beta_new

def phase_detector(v_alpha, v_beta, theta):
    cos_theta = math.cos(theta)
    sin_theta = math.sin(theta)
    return v_alpha * sin_theta - v_beta * cos_theta

def PI_update(error, state):
    state.integrator += Ki * Ts * error
    state.output = Kp * error + state.integrator

def main():
    sogi = SOGI_State()
    pi = PI_State()
    theta_pll = 0.0
    w_pll = w0
    
    total_samples = int(0.1 / Ts)
    samples_per_cycle = int(1.0 / (50.0 * Ts))
    print_samples = 5 * samples_per_cycle
    
    print("时间(s)\t输入相位(rad)\tPLL相位(rad)\t频率估计(Hz)")
    
    for i in range(total_samples):
        t = i * Ts
        v_input = math.sin(w0 * t)
        
        SOGI_update(v_input, sogi)
        
        error = phase_detector(sogi.v_alpha, sogi.v_beta, theta_pll)
        
        PI_update(error, pi)
        
        w_pll = w0 + pi.output
        theta_pll += w_pll * Ts
        
        if theta_pll > 2.0 * PI:
            theta_pll -= 2.0 * PI
        
        if i < print_samples:
            input_phase = math.fmod(w0 * t, 2.0 * PI)
            print(f"{t:.6f}\t{input_phase:.6f}\t{theta_pll:.6f}\t{w_pll / (2.0 * PI):.6f}")

if __name__ == "__main__":
    main()