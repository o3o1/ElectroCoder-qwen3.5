import math 
def generate_spwm(amplitude, frequency, carrier_freq, samples): 
    duty_cycle = [] 
    for n in range(samples): 
        t = n / carrier_freq 
        sine_value = amplitude * math.sin(2 * math.pi * frequency * t) 
        duty = 0.5 + 0.5 * sine_value 
