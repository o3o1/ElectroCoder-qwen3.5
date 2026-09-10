import math
import random
import time

PI = 3.14159265358979323846
FS = 2000.0
R = 0.95

class FilterCoeff:
    def __init__(self):
        self.b0 = 0.0
        self.b1 = 0.0
        self.b2 = 0.0
        self.a1 = 0.0
        self.a2 = 0.0

def notch_filter(input_val, state_x, state_y, b0, b1, b2, a1, a2):
    output = b0 * input_val + b1 * state_x[0] + b2 * state_x[1] - a1 * state_y[0] - a2 * state_y[1]
    
    state_x[1] = state_x[0]
    state_x[0] = input_val
    
    state_y[1] = state_y[0]
    state_y[0] = output
    
    return output

def calculate_notch_coeff(freq, coeff):
    omega0 = 2.0 * PI * freq / FS
    cos_omega0 = math.cos(omega0)
    
    coeff.b0 = 1.0
    coeff.b1 = -2.0 * cos_omega0
    coeff.b2 = 1.0
    
    coeff.a1 = -2.0 * R * cos_omega0
    coeff.a2 = R * R

def generate_test_signal(n):
    t = n / FS
    signal = 0.0
    
    signal += 1.0 * math.sin(2.0 * PI * 50.0 * t)
    signal += 0.3 * math.sin(2.0 * PI * 150.0 * t)
    signal += 0.2 * math.sin(2.0 * PI * 250.0 * t)
    signal += 0.15 * math.sin(2.0 * PI * 350.0 * t)
    
    signal += 0.05 * (random.random() - 0.5)
    
    return signal

def main():
    random.seed(12345)
    
    coeff_3rd = FilterCoeff()
    coeff_5th = FilterCoeff()
    coeff_7th = FilterCoeff()
    
    calculate_notch_coeff(150.0, coeff_3rd)
    calculate_notch_coeff(250.0, coeff_5th)
    calculate_notch_coeff(350.0, coeff_7th)
    
    state_x1 = [0.0, 0.0]
    state_y1 = [0.0, 0.0]
    state_x2 = [0.0, 0.0]
    state_y2 = [0.0, 0.0]
    state_x3 = [0.0, 0.0]
    state_y3 = [0.0, 0.0]
    
    print('PLL Harmonic Rejection Digital Filter Test')
    print(f'Sampling Frequency: {FS:.0f} Hz')
    print('Reject Harmonics: 150Hz(3rd), 250Hz(5th), 350Hz(7th)')
    print(f'Pole Radius: {R:.2f}\n')
    
    for i in range(20):
        input_val = generate_test_signal(i)
        
        output1 = notch_filter(input_val, state_x1, state_y1,
                             coeff_3rd.b0, coeff_3rd.b1, coeff_3rd.b2,
                             coeff_3rd.a1, coeff_3rd.a2)
        
        output2 = notch_filter(output1, state_x2, state_y2,
                             coeff_5th.b0, coeff_5th.b1, coeff_5th.b2,
                             coeff_5th.a1, coeff_5th.a2)
        
        output3 = notch_filter(output2, state_x3, state_y3,
                             coeff_7th.b0, coeff_7th.b1, coeff_7th.b2,
                             coeff_7th.a1, coeff_7th.a2)
        
        print(f'Sample {i}: Input={input_val:.6f}, Output={output3:.6f}')

if __name__ == '__main__':
    main()