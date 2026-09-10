import math

PI = 3.14159265358979323846

class PWM_Config:
    def __init__(self):
        self.switching_freq = 20000
        self.carrier_freq = 1000000
        self.modulation_index = 80
        self.filter_coeffs = [0.25, 0.5, 0.25]
        self.filter_order = 3

def apply_software_filter(config, raw_samples):
    order = config.filter_order
    coeffs = config.filter_coeffs
    sample_count = len(raw_samples)
    filtered_samples = [0.0] * sample_count
    
    for n in range(sample_count):
        sum_val = 0.0
        for k in range(order):
            if n >= k:
                sum_val += coeffs[k] * raw_samples[n - k]
        filtered_samples[n] = sum_val
    
    return filtered_samples

def main():
    config = PWM_Config()
    sample_count = 100
    raw_samples = []
    
    for i in range(sample_count):
        t = i / 1000.0
        signal = math.sin(2.0 * PI * 1000.0 * t)
        noise = 0.1 * math.sin(2.0 * PI * 50000.0 * t)
        raw_samples.append(signal + noise)
    
    filtered_samples = apply_software_filter(config, raw_samples)
    
    for i in range(10):
        print(f"原始采样[{i}]={raw_samples[i]:.6f}, 滤波后[{i}]={filtered_samples[i]:.6f}")

if __name__ == "__main__":
    main()