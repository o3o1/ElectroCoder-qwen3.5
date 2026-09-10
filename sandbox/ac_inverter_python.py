import math
import random
import time

print("中央空调变频器温度闭环控制模拟")
print("=" * 32)

# PID参数
PID_KP = 2.0
PID_KI = 0.1
PID_KD = 1.0
TEMP_DEADBAND = 0.5
SAMPLE_PERIOD = 1.0
MIN_FREQ = 0.0
MAX_FREQ = 50.0
SIMULATION_TIME = 30

# 硬件寄存器模拟宏定义
class MockHardware:
    def __init__(self):
        self.temp_sensor_reg = 25.0  # 初始温度25℃
        self.inverter_freq_reg = 0.0  # 初始频率0Hz
    
    def read_temperature(self):
        return self.temp_sensor_reg
    
    def set_frequency(self, freq):
        self.inverter_freq_reg = freq

# PID控制器结构体
class PIDController:
    def __init__(self, Kp, Ki, Kd, min_out, max_out):
        self.Kp = Kp
        self.Ki = Ki
        self.Kd = Kd
        self.integral = 0.0
        self.prev_error = 0.0
        self.output_min = min_out
        self.output_max = max_out
    
    def update(self, setpoint, measurement, dt):
        error = setpoint - measurement
        
        # 死区处理
        if abs(error) <= TEMP_DEADBAND:
            self.integral = 0.0
            self.prev_error = error
            return 0.0
        
        # 积分项
        self.integral += error * dt
        
        # 微分项
        derivative = (error - self.prev_error) / dt
        
        # PID输出
        output = self.Kp * error + self.Ki * self.integral + self.Kd * derivative
        
        # 限幅
        if output > self.output_max:
            output = self.output_max
        if output < self.output_min:
            output = self.output_min
        
        self.prev_error = error
        
        return output

# 温度传感器读取函数（模拟）
def mock_read_temperature(setpoint, current_temp, output_freq, time_step):
    # 模拟温度变化
    temp_change = 0.0
    
    if output_freq > 0:
        if setpoint < current_temp:  # 降温
            temp_change = -0.1 * output_freq / 50.0
        else:  # 升温
            temp_change = 0.08 * output_freq / 50.0
    
    # 模拟环境干扰
    ambient_influence = 0.0
    if time_step > 10 and time_step < 20:
        ambient_influence = 0.05  # 模拟环境温度波动
    
    # 模拟传感器噪声
    noise = (random.randint(0, 100) - 50) / 1000.0
    
    new_temp = current_temp + temp_change + ambient_influence + noise
    
    # 温度范围限制
    if new_temp < 15.0:
        new_temp = 15.0
    if new_temp > 35.0:
        new_temp = 35.0
    
    return new_temp

# 变频器控制函数（模拟）
def mock_set_inverter_frequency(freq):
    # 在实际硬件中，这里会写入寄存器
    # INVERTER_FREQ_REG = freq
    pass

# 运行测试场景
def run_test_scenario(initial_temp, setpoint, scenario_name):
    print(f'\n=== {scenario_name} ===')
    print(f'初始温度: {initial_temp:.1f}℃, 设定温度: {setpoint:.1f}℃\n')
    print('时间(s)  设定温度  实际温度  误差    频率(Hz)')
    print('-' * 44)
    
    # 初始化PID控制器
    pid = PIDController(PID_KP, PID_KI, PID_KD, MIN_FREQ, MAX_FREQ)
    
    current_temp = initial_temp
    stable_time = 0
    max_overshoot = 0.0
    is_stable = False
    stable_start = -1
    
    for t in range(SIMULATION_TIME):
        # 计算误差
        error = setpoint - current_temp
        
        # PID计算输出频率
        output_freq = pid.update(setpoint, current_temp, SAMPLE_PERIOD)
        
        # 设置变频器频率
        mock_set_inverter_frequency(output_freq)
        
        # 打印状态
        print(f'{t:6d}   {setpoint:7.1f}   {current_temp:7.1f}   {error:6.2f}   {output_freq:8.1f}')
        
        # 判断是否稳定
        if abs(error) <= 0.2:  # 误差小于0.2℃认为稳定
            if not is_stable:
                is_stable = True
                stable_start = t
        else:
            is_stable = False
            stable_start = -1
        
        # 计算超调量
        if error * (setpoint - initial_temp) < 0:
            current_overshoot = abs(error)
            if current_overshoot > max_overshoot:
                max_overshoot = current_overshoot
        
        # 读取下一时刻温度
        current_temp = mock_read_temperature(setpoint, current_temp, output_freq, t)
        
        # 记录稳定时间
        if t == SIMULATION_TIME - 1 and stable_start != -1:
            stable_time = stable_start
    
    print(f'\n统计信息:')
    print(f'稳定时间: {stable_time}秒')
    print(f'最大超调量: {max_overshoot:.2f}℃')
    print(f'最终误差: {setpoint - current_temp:.2f}℃')

# 主函数
def main():
    random.seed(time.time())
    
    # 测试场景A: 降温过程
    run_test_scenario(25.0, 22.0, '测试场景A: 降温过程(25℃→22℃)')
    
    # 测试场景B: 升温过程
    run_test_scenario(18.0, 22.0, '测试场景B: 升温过程(18℃→22℃)')
    
    # 测试场景C: 温度波动场景
    print('\n=== 测试场景C: 温度波动场景 ===')
    print('阶段1: 25℃→22℃, 阶段2: 22℃→24℃, 阶段3: 24℃→22℃\n')
    
    pid = PIDController(PID_KP, PID_KI, PID_KD, MIN_FREQ, MAX_FREQ)
    
    current_temp = 25.0
    setpoint = 22.0
    phase = 1
    
    print('时间(s)  设定温度  实际温度  误差    频率(Hz)  阶段')
    print('-' * 55)
    
    for t in range(SIMULATION_TIME):
        # 阶段切换
        if t == 10:
            setpoint = 24.0
            phase = 2
            pid.integral = 0.0  # 重置积分项
        elif t == 20:
            setpoint = 22.0
            phase = 3
            pid.integral = 0.0  # 重置积分项
        
        # 计算误差和输出
        error = setpoint - current_temp
        output_freq = pid.update(setpoint, current_temp, SAMPLE_PERIOD)
        
        # 设置频率
        mock_set_inverter_frequency(output_freq)
        
        # 打印状态
        print(f'{t:6d}   {setpoint:7.1f}   {current_temp:7.1f}   {error:6.2f}   {output_freq:8.1f}   {phase}')
        
        # 读取下一时刻温度
        current_temp = mock_read_temperature(setpoint, current_temp, output_freq, t)
    
    print('\n=== 测试完成 ===')

if __name__ == '__main__':
    main()