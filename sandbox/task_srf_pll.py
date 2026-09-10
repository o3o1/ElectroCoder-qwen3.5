import math

PI = 3.14159265358979323846
KP = 100.0
KI = 1000.0
DT = 0.001
INTEGRAL_LIMIT = 1000.0

def clarke_transform(a, b, c):
    alpha = a
    beta = (b - c) / math.sqrt(3.0)
    return alpha, beta

def park_transform(alpha, beta, theta):
    cos_theta = math.cos(theta)
    sin_theta = math.sin(theta)
    d = alpha * cos_theta + beta * sin_theta
    q = -alpha * sin_theta + beta * cos_theta
    return d, q

class PLLController:
    def __init__(self):
        self.integral = 0.0
        
    def pi_controller(self, error, kp, ki):
        self.integral += error * DT
        if self.integral > INTEGRAL_LIMIT:
            self.integral = INTEGRAL_LIMIT
        if self.integral < -INTEGRAL_LIMIT:
            self.integral = -INTEGRAL_LIMIT
        return kp * error + ki * self.integral
    
    def pll_update(self, va, vb, vc, theta, frequency):
        alpha, beta = clarke_transform(va, vb, vc)
        d, q = park_transform(alpha, beta, theta)
        
        error = -q
        delta_freq = self.pi_controller(error, KP, KI)
        
        new_frequency = 50.0 + delta_freq
        new_theta = theta + 2.0 * PI * new_frequency * DT
        new_theta = new_theta % (2.0 * PI)
        
        return new_theta, new_frequency

def main():
    theta_grid = 0.0
    theta_pll = 0.0
    freq_pll = 50.0
    pll = PLLController()
    
    for i in range(5):
        va = math.sin(theta_grid)
        vb = math.sin(theta_grid - 2.0 * PI / 3.0)
        vc = math.sin(theta_grid + 2.0 * PI / 3.0)
        
        theta_pll, freq_pll = pll.pll_update(va, vb, vc, theta_pll, freq_pll)
        
        print(f"Step {i+1}: theta_grid={theta_grid:.6f}, theta_pll={theta_pll:.6f}, freq={freq_pll:.6f}")
        
        theta_grid += 0.1

if __name__ == "__main__":
    main()