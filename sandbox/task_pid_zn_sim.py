import math
import numpy as np

class PIDController:
    def __init__(self, kp, ki, kd):
        self.kp = kp
        self.ki = ki
        self.kd = kd
        self.integral = 0.0
        self.prev_error = 0.0
    
    def update(self, error, dt):
        self.integral += error * dt
        derivative = (error - self.prev_error) / dt
        output = self.kp * error + self.ki * self.integral + self.kd * derivative
        self.prev_error = error
        return output

class Plant:
    def __init__(self, a, b):
        self.a = a
        self.b = b
        self.prev_output = 0.0
    
    def update(self, input_val):
        output = self.a * self.prev_output + self.b * input_val
        self.prev_output = output
        return output

def detect_oscillation(output_history, sample_time=0.01):
    count = len(output_history)
    if count < 100:
        return False, 0.0, 0.0
    
    max_val = -1e9
    min_val = 1e9
    max_indices = []
    min_indices = []
    
    for i in range(50, count - 1):
        if output_history[i] > output_history[i-1] and output_history[i] > output_history[i+1]:
            if len(max_indices) < 20:
                max_indices.append(i)
                if output_history[i] > max_val:
                    max_val = output_history[i]
        if output_history[i] < output_history[i-1] and output_history[i] < output_history[i+1]:
            if len(min_indices) < 20:
                min_indices.append(i)
                if output_history[i] < min_val:
                    min_val = output_history[i]
    
    if len(max_indices) < 3 or len(min_indices) < 3:
        return False, 0.0, 0.0
    
    amplitude = (max_val - min_val) / 2.0
    
    period_sum = 0.0
    period_count = 0
    
    for i in range(1, len(max_indices)):
        p = (max_indices[i] - max_indices[i-1]) * sample_time
        if p > 0.1:
            period_sum += p
            period_count += 1
    
    if period_count < 2:
        return False, 0.0, 0.0
    
    period = period_sum / period_count
    
    amplitude_std = 0.0
    last_peak = abs(output_history[max_indices[0]])
    for i in range(1, len(max_indices)):
        current_peak = abs(output_history[max_indices[i]])
        amplitude_std += abs(current_peak - last_peak)
        last_peak = current_peak
    
    amplitude_std /= (len(max_indices) - 1)
    
    is_oscillating = (amplitude_std / amplitude < 0.01)
    return is_oscillating, amplitude, period

def zn_tuning(plant_a, plant_b, setpoint):
    max_samples = 10000
    sample_time = 0.01
    kp_start = 0.1
    kp_step = 0.1
    kp_max = 50.0
    
    for kp in np.arange(kp_start, kp_max + kp_step, kp_step):
        pid = PIDController(kp, 0.0, 0.0)
        plant = Plant(plant_a, plant_b)
        
        output_history = []
        output = 0.0
        
        for i in range(max_samples):
            error = setpoint - output
            control = pid.update(error, sample_time)
            output = plant.update(control)
            output_history.append(output)
            
            if i > 500:
                is_osc, amplitude, period = detect_oscillation(output_history, sample_time)
                if is_osc:
                    return kp, period
    
    return 0.0, 0.0

def print_results(ku, tu, test_case):
    print(f"测试用例{test_case}：")
    print(f"临界比例系数Ku = {ku:.2f}, 临界周期Tu = {tu:.2f}s")
    print(f"P控制器: Kp={0.5 * ku:.2f}")
    print(f"PI控制器: Kp={0.45 * ku:.2f}, Ti={tu / 1.2:.2f}s")
    print(f"PID控制器: Kp={0.6 * ku:.2f}, Ti={tu / 2.0:.2f}s, Td={tu / 8.0:.2f}s")
    print()

def main():
    print("Ziegler-Nichols临界比例度法PID参数自整定")
    print("========================================")
    print()
    
    ku1, tu1 = zn_tuning(0.8, 0.2, 1.0)
    print_results(ku1, tu1, 1)
    
    ku2, tu2 = zn_tuning(0.7, 0.3, 2.0)
    print_results(ku2, tu2, 2)

if __name__ == "__main__":
    main()