import math

PI = 3.1415926535

def iir_lowpass(input_val, prev_output, alpha):
    output = alpha * input_val + (1.0 - alpha) * prev_output
    return output, output

def main():
    fs = 100.0
    N = 100
    t = [i / fs for i in range(N)]
    signal = [math.sin(2.0 * PI * 1.0 * t[i]) + 0.3 * math.sin(2.0 * PI * 10.0 * t[i]) for i in range(N)]
    
    prev_output1 = 0.0
    prev_output2 = 0.0
    filtered1 = []
    filtered2 = []
    
    for i in range(N):
        output1, prev_output1 = iir_lowpass(signal[i], prev_output1, 0.1)
        output2, prev_output2 = iir_lowpass(signal[i], prev_output2, 0.5)
        filtered1.append(output1)
        filtered2.append(output2)
    
    print('t=0.00, raw=0.000, alpha0.1=0.000, alpha0.5=0.000')
    for i in range(10):
        print(f't={t[i]:.2f}, raw={signal[i]:.3f}, alpha0.1={filtered1[i]:.3f}, alpha0.5={filtered2[i]:.3f}')

if __name__ == '__main__':
    main()