#!/usr/bin/env python3
# 模拟C代码逻辑

class IncrementalPID:
    def __init__(self):
        self.e_k_1 = 0.0
        self.e_k_2 = 0.0
    
    def calculate(self, setpoint, measured, kp, ki, kd):
        e_k = setpoint - measured
        
        delta_u = kp * (e_k - self.e_k_1) + ki * e_k + kd * (e_k - 2.0 * self.e_k_1 + self.e_k_2)
        
        # 更新历史误差
        self.e_k_2 = self.e_k_1
        self.e_k_1 = e_k
        
        return delta_u

def main():
    pid = IncrementalPID()
    setpoint = 100.0
    measured = 0.0
    kp = 0.8
    ki = 0.05
    kd = 0.1
    
    for i in range(1, 6):
        delta_u = pid.calculate(setpoint, measured, kp, ki, kd)
        measured += delta_u
        
        print(f'Loop {i}: measured={measured:.2f}, delta_u={delta_u:.2f}')

if __name__ == "__main__":
    main()