import math

PI = 3.14159265358979323846
OMEGA0 = 2.0 * PI * 50.0
KP = 100.0
KI = 2000.0

def park_transform(a, b, c, theta):
    cos_theta = math.cos(theta)
    sin_theta = math.sin(theta)
    cos_theta_120 = math.cos(theta - 2.0*PI/3.0)
    sin_theta_120 = math.sin(theta - 2.0*PI/3.0)
    cos_theta_240 = math.cos(theta + 2.0*PI/3.0)
    sin_theta_240 = math.sin(theta + 2.0*PI/3.0)
    
    d = (2.0/3.0) * (cos_theta * a + cos_theta_120 * b + cos_theta_240 * c)
    q = (2.0/3.0) * (-sin_theta * a - sin_theta_120 * b - sin_theta_240 * c)
    return d, q

class PLL:
    def __init__(self):
        self.integral_vq = 0.0
        
    def update(self, v_d, v_q, theta, omega, dt):
        self.integral_vq += v_q * dt
        omega_new = OMEGA0 + KP * v_q + KI * self.integral_vq
        theta_new = theta + omega_new * dt
        
        while theta_new > 2.0 * PI:
            theta_new -= 2.0 * PI
        while theta_new < 0:
            theta_new += 2.0 * PI
            
        return theta_new, omega_new

def main():
    dt = 0.001
    theta_grid = 0.0
    theta_pll = 0.0
    omega_pll = OMEGA0
    pll = PLL()
    
    print('Time(s)  Grid_theta(rad)  PLL_theta(rad)  PLL_freq(Hz)')
    
    for i in range(5):
        time = i * dt
        
        va = math.sin(theta_grid)
        vb = math.sin(theta_grid - 2.0*PI/3.0)
        vc = math.sin(theta_grid + 2.0*PI/3.0)
        
        v_d, v_q = park_transform(va, vb, vc, theta_pll)
        theta_pll, omega_pll = pll.update(v_d, v_q, theta_pll, omega_pll, dt)
        
        print(f'{time:.3f}    {theta_grid:.3f}            {theta_pll:.3f}           {omega_pll/(2.0*PI):.3f}')
        
        theta_grid += OMEGA0 * dt
        while theta_grid > 2.0 * PI:
            theta_grid -= 2.0 * PI

if __name__ == '__main__':
    main()