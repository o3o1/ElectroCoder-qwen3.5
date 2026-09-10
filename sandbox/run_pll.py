import math
import sys

PI = 3.14159265358979323846
K = 1.414
GAMMA = 100.0

class SinglePhasePLL:
    def __init__(self, init_freq):
        self.theta = 0.0
        self.freq = init_freq
        self.v_alpha = 0.0
        self.v_beta = 0.0
        self.integrator1 = 0.0
        self.integrator2 = 0.0
        self.freq_integrator = 0.0

def pll_init(pll, init_freq):
    pll.theta = 0.0
    pll.freq = init_freq
    pll.v_alpha = 0.0
    pll.v_beta = 0.0
    pll.integrator1 = 0.0
    pll.integrator2 = 0.0
    pll.freq_integrator = 0.0

def pll_update(pll, v_input, dt):
    omega = 2.0 * PI * pll.freq
    
    sogi_error = v_input - pll.v_alpha
    pll.integrator1 += (K * omega * sogi_error - omega * pll.v_beta) * dt
    pll.v_alpha = pll.integrator1
    
    pll.integrator2 += omega * pll.v_alpha * dt
    pll.v_beta = pll.integrator2
    
    freq_error = v_input * pll.v_beta
    pll.freq_integrator += GAMMA * freq_error * dt
    pll.freq += pll.freq_integrator
    
    if pll.freq < 0.0:
        pll.freq = 0.0
    
    pll.theta += 2.0 * PI * pll.freq * dt
    while pll.theta > 2.0 * PI:
        pll.theta -= 2.0 * PI
    while pll.theta < 0.0:
        pll.theta += 2.0 * PI

def main():
    pll = SinglePhasePLL(50.0)
    
    t = 0.0
    dt = 0.0001
    total_time = 0.1
    print_interval = 100
    step_count = 0
    
    print('Time(s)\tInput(V)\tPhase(deg)\tFreq(Hz)')
    
    while t < total_time + dt/2.0:
        v_input = 220.0 * math.sin(2.0 * PI * 50.0 * t + PI/4.0)
        
        pll_update(pll, v_input, dt)
        
        if step_count % print_interval == 0:
            phase_deg = pll.theta * 180.0 / PI
            print(f'{t:.4f}\t{v_input:.2f}\t\t{phase_deg:.2f}\t\t{pll.freq:.2f}')
        
        t += dt
        step_count += 1

if __name__ == '__main__':
    main()