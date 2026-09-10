#!/usr/bin/env python3
import subprocess
import sys

# 模拟C程序的逻辑
class SlidingAverageFilter:
    def __init__(self, size):
        self.buffer = [0] * size
        self.size = size
        self.index = 0
        self.sum = 0
        self.is_full = False
    
    def update(self, new_value):
        # 减去即将被覆盖的旧值
        if self.is_full:
            self.sum -= self.buffer[self.index]
        
        # 添加新值
        self.sum += new_value
        self.buffer[self.index] = new_value
        
        # 更新索引
        self.index = (self.index + 1) % self.size
        
        # 检查缓冲区是否已满
        if not self.is_full and self.index == 0:
            self.is_full = True
        
        # 计算平均值
        count = self.size if self.is_full else self.index
        return self.sum // count
    
    def get_average(self):
        count = self.size if self.is_full else self.index
        if count == 0:
            return 0
        return self.sum // count
    
    def print_buffer(self):
        print("Buffer data:", " ".join(str(x) for x in self.buffer))

def main():
    # 创建大小为5的滑动平均滤波器
    filter = SlidingAverageFilter(5)
    
    # 模拟ADC采样数据序列
    adc_samples = [10, 20, 30, 40, 50, 60, 70, 80]
    
    print("Sliding Average Filter Test (Buffer size: 5)")
    print("========================================")
    
    # 依次输入每个采样值并打印滤波结果
    for sample in adc_samples:
        filtered_value = filter.update(sample)
        print(f"Input: {sample}, Filtered: {filtered_value}")
    
    print()
    print(f"Current average: {filter.get_average()}")
    filter.print_buffer()

if __name__ == "__main__":
    main()