#!/usr/bin/env python3
# Python模拟运行C代码

def iir_lowpass(x, prev_y, alpha):
    y = alpha * x + (1 - alpha) * prev_y
    return y, y  # 返回输出和新的历史值

def main():
    input_data = [1.0, 0.5, 0.2, 0.8, 0.3, 0.9, 0.1, 0.7]
    alpha = 0.3
    prev_y = 0.0
    sum_output = 0.0
    
    for i, x in enumerate(input_data):
        output, prev_y = iir_lowpass(x, prev_y, alpha)
        print(f'Input[{i}]={x:.2f}, Output[{i}]={output:.4f}')
        sum_output += output
    
    average = sum_output / len(input_data)
    print(f'Average output: {average:.4f}')

if __name__ == "__main__":
    main()