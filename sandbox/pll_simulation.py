import math

PI = math.pi
PI_2_3 = 2.0 * PI / 3.0
TS = 0.001

class ThreePhase:
    def __init__(self, va=0, vb=0, vc=0):
        self.va = va
        self.vb = vb
        self.vc = vc

class AlphaBeta:
    def __init__(self, alpha=0, beta=0):
        self.alpha = alpha
        self.beta = beta

class DQ:
    def __init__(self, d=0, q=0):
        self.d = d
        self.q = q

class PIController:
    def __init__(self, kp=0, ki=0):
        self.kp = kp
        self.ki = ki
        self.integral = 0.0
        self.error = 0.0
        self.output = 0.0

class PLLState:
    def __init__(self):
        self.theta_est = 0.0
        self.omega_est = 2.0 * PI * 50.0
        self.freq_est = 50.0
        self.pi = PIController(100.0, 1000.0)

def clarke_transform(abc, alphabeta):
    alphabeta.alpha = abc.va
    alphabeta.beta = (abc.vb - abc.vc) / math.sqrt(3.0)

def park_transform(alphabeta, theta, dq):
    cos_theta = math.cos(theta)
    sin_theta = math.sin(theta)
    
    dq.d = alphabeta.alpha * cos_theta + alphabeta.beta * sin_theta
    dq.q = -alphabeta.alpha * sin_theta + alphabeta.beta * cos_theta

def pi_controller_update(pi, error):
    pi.error = error
    pi.integral += error * TS
    pi.output = pi.kp * error + pi.ki * pi.integral

def pll_update(pll, voltage):
    alphabeta = AlphaBeta()
    dq = DQ()
    
    clarke_transform(voltage, alphabeta)
    park_transform(alphabeta, pll.theta_est, dq)
    
    pi_controller_update(pll.pi, dq.q)
    
    pll.omega_est = pll.pi.output
    pll.theta_est += pll.omega_est * TS
    
    while pll.theta_est > 2.0 * PI:
        pll.theta_est -= 2.0 * PI
    while pll.theta_est < 0:
        pll.theta_est += 2.0 * PI
    
    pll.freq_est = pll.omega_est / (2.0 * PI)

def main():
    pll = PLLState()
    
    print("SRF-PLL Test")
    print("Iter\tTheta_in(rad)\tTheta_est(rad)\tFreq_est(Hz)")
    print("---------------------------------------------------")
    
    for i in range(5):
        theta_input = i * PI / 6.0
        
        voltage = ThreePhase(
            math.sin(theta_input),
            math.sin(theta_input - PI_2_3),
            math.sin(theta_input + PI_2_3)
        )
        
        pll_update(pll, voltage)
        
        print(f"{i+1}\t{theta_input:.4f}\t\t{pll.theta_est:.4f}\t\t{pll.freq_est:.2f}")

if __name__ == "__main__":
    main()