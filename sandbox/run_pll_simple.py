import math

PI = 3.14159265358979323846
TS = 0.0001
F_GRID = 50.0
A = 1.0
INIT_PHASE = 0.2
K_SOGI = 1.414
KP = 100.0
KI = 1000.0
OMEGA_0 = 2.0 * PI * F_GRID

class SOGI_State:
    def __init__(self):
        self.u = 0.0
        self.u_quad = 0.0
        self.v1 = 0.0
        self.v2 = 0.0

class PLL_State:
    def __init__(self):
        self.error = 0.0
        self.integral = 0.0
        self.omega_est = OMEGA_0
        self.theta_est = 0.0

def sogi_update(s, input_val, omega, ts):
    k = K_SOGI
    v1_new = s.v1 + ts * (omega * s.v2 - k * omega * (s.v1 - input_val))
    v2_new = s.v2 + ts * (-omega * s.v1)
    
    s.v1 = v1_new
    s.v2 = v2_new
    s.u = s.v1
    s.u_quad = s.v2

def pll_update(p, u, u_quad, ts):
    p.error = u * u_quad
    
    p.integral += p.error * ts
    
    p.omega_est = OMEGA_0 + KP * p.error + KI * p.integral
    
    p.theta_est += p.omega_est * ts
    
    if p.theta_est > 2.0 * PI:
        p.theta_est -= 2.0 * PI
    if p.theta_est < 0:
        p.theta_est += 2.0 * PI

sogi = SOGI_State()
pll = PLL_State()

print('采样点, 输入电压, 估计频率(Hz), 估计相位(弧度)')

for n in range(1000):
    t = n * TS
    u_input = A * math.sin(2.0 * PI * F_GRID * t + INIT_PHASE)
    
    sogi_update(sogi, u_input, pll.omega_est, TS)
    pll_update(pll, sogi.u, sogi.u_quad, TS)
    
    if n < 10:
        print(f'{n}, {u_input:.6f}, {pll.omega_est/(2.0*PI):.6f}, {pll.theta_est:.6f}')