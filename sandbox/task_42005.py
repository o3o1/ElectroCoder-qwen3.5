def iir_lowpass(input_val, prev_output, alpha):
    output = alpha * input_val + (1 - alpha) * prev_output
    return output, output

def main():
    test_signal = [1.0, 1.2, 0.8, 1.5, 0.5, 1.3, 0.7, 1.1, 0.9, 1.0]
    n = len(test_signal)
    
    print("原始信号: ", end="")
    for i in range(n):
        print(f"{test_signal[i]:.2f} ", end="")
    print()
    
    prev_output1 = test_signal[0]
    print("alpha=0.3滤波后: ", end="")
    print(f"{prev_output1:.2f} ", end="")
    for i in range(1, n):
        filtered, prev_output1 = iir_lowpass(test_signal[i], prev_output1, 0.3)
        print(f"{filtered:.2f} ", end="")
    print()
    
    prev_output2 = test_signal[0]
    print("alpha=0.7滤波后: ", end="")
    print(f"{prev_output2:.2f} ", end="")
    for i in range(1, n):
        filtered, prev_output2 = iir_lowpass(test_signal[i], prev_output2, 0.7)
        print(f"{filtered:.2f} ", end="")
    print()

if __name__ == "__main__":
    main()