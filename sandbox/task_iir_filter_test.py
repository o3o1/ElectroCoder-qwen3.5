import math

def iir_lowpass_filter(input_val, alpha, prev_output):
    output = alpha * input_val + (1.0 - alpha) * prev_output
    return output, output

def main():
    fs = 1000.0
    T = 1.0 / fs
    fc = 50.0
    
    alpha = (2.0 * math.pi * fc * T) / (2.0 * math.pi * fc * T + 1.0)
    
    prev_output = 0.0
    
    print(f"Sampling frequency: {fs} Hz")
    print(f"Cutoff frequency: {fc} Hz")
    print(f"Alpha coefficient: {alpha:.6f}\n")
    
    print("Index\tOriginal\tFiltered")
    print("-" * 32)
    
    for n in range(100):
        x = math.sin(2.0 * math.pi * 10.0 * n * T) + 0.5 * math.sin(2.0 * math.pi * 100.0 * n * T)
        y, prev_output = iir_lowpass_filter(x, alpha, prev_output)
        
        if n < 10:
            print(f"{n}\t{x:.6f}\t{y:.6f}")

if __name__ == "__main__":
    main()