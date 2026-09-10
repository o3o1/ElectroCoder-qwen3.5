#!/usr/bin/env python3
# C语言程序的Python模拟版本

WINDOW_SIZE = 10

class SlidingWindow:
    def __init__(self):
        self.buffer = [0] * WINDOW_SIZE
        self.index = 0
        self.count = 0
        self.sum = 0
    
    def sliding_average_filter(self, new_sample):
        # 如果窗口已满，先减去即将被替换的旧值
        if self.count == WINDOW_SIZE:
            self.sum -= self.buffer[self.index]
        else:
            self.count += 1
        
        # 添加新值到窗口
        self.buffer[self.index] = new_sample
        self.sum += new_sample
        
        # 更新索引（循环缓冲区）
        self.index = (self.index + 1) % WINDOW_SIZE
        
        # 计算平均值（整数除法）
        return self.sum // self.count

def main():
    window = SlidingWindow()
    
    # 模拟ADC采样值
    adc_samples = [100, 105, 98, 102, 110, 95, 103, 107, 99, 101]
    
    print("初始10个采样值的滤波过程：")
    for sample in adc_samples:
        filtered_value = window.sliding_average_filter(sample)
        print(f"采样值: {sample}, 滤波后平均值: {filtered_value}")
    
    # 额外处理5个新采样值
    new_samples = [104, 96, 108, 97, 105]
    
    print("\n额外5个新采样值的滤波过程：")
    for sample in new_samples:
        filtered_value = window.sliding_average_filter(sample)
        print(f"采样值: {sample}, 滤波后平均值: {filtered_value}")

if __name__ == "__main__":
    main()