import math

def calculate_chebyshev_coeffs(fc, fs, Rp):
    PI = math.pi
    wp = 2.0 * PI * fc
    T = 1.0 / fs
    
    epsilon = math.sqrt(10.0**(Rp/10.0) - 1.0)
    beta = math.asinh(1.0/epsilon) / 2.0
    eta = math.sinh(beta)
    xi = math.cosh(beta)
    
    wc = 2.0 / T * math.tan(wp * T / 2.0)
    
    s1_real = -eta * wc
    s1_imag = xi * wc
    s2_real = -eta * wc
    s2_imag = -xi * wc
    
    z1_real = (1.0 + s1_real * T/2.0) / (1.0 - s1_real * T/2.0)
    z1_imag = (s1_imag * T/2.0) / (1.0 - s1_real * T/2.0)
    z2_real = (1.0 + s2_real * T/2.0) / (1.0 - s2_real * T/2.0)
    z2_imag = (s2_imag * T/2.0) / (1.0 - s2_real * T/2.0)
    
    z1_mag_sq = z1_real*z1_real + z1_imag*z1_imag
    z2_mag_sq = z2_real*z2_real + z2_imag*z2_imag
    
    b0 = 1.0
    b1 = -2.0 * z1_real
    b2 = z1_mag_sq
    
    a0 = 1.0
    a1 = -2.0 * z2_real
    a2 = z2_mag_sq
    
    norm = a0
    b0 /= norm
    b1 /= norm
    b2 /= norm
    a1 /= norm
    a2 /= norm
    
    return [b0, b1, b2], [1.0, a1, a2]

def chebyshev_filter(input_val, state, coeff):
    b0, b1, b2, a1, a2 = coeff
    
    w0 = input_val - a1 * state[0] - a2 * state[1]
    output = b0 * w0 + b1 * state[0] + b2 * state[1]
    
    state[1] = state[0]
    state[0] = w0
    
    return output

def main():
    fc = 100.0
    fs = 1000.0
    Rp = 1.0
    
    b, a = calculate_chebyshev_coeffs(fc, fs, Rp)
    
    print("Chebyshev Type I Lowpass Filter Coefficients:")
    print(f"b0 = {b[0]:.6f}, b1 = {b[1]:.6f}, b2 = {b[2]:.6f}")
    print(f"a0 = 1.000000, a1 = {a[1]:.6f}, a2 = {a[2]:.6f}")
    
    coeff = [b[0], b[1], b[2], a[1], a[2]]
    state = [0.0, 0.0]
    
    print("\nTest Signal (50Hz + 150Hz sine waves):")
    print("Input samples and filtered output:")
    
    for i in range(10):
        t = i / fs
        input_val = math.sin(2.0 * math.pi * 50.0 * t) + math.sin(2.0 * math.pi * 150.0 * t)
        output = chebyshev_filter(input_val, state, coeff)
        
        print(f"Sample {i}: Input = {input_val:.6f}, Output = {output:.6f}")

if __name__ == "__main__":
    main()