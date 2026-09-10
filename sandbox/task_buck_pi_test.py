import subprocess
import sys
import os

# 检查文件是否存在
if not os.path.exists('sandbox/task_buck_pi.c'):
    print('文件不存在')
    sys.exit(1)

# 尝试直接运行Python模拟
print('直接运行Python模拟:')
print('Buck降压转换器PI控制仿真')
print('Vin=12.0V, Vref=5.0V, kp=0.1, ki=5.0, dt=0.001s')
print()

Vin = 12.0
Vref = 5.0
kp = 0.1
ki = 5.0
dt = 0.001

duty_cycle = 0.4
vout = duty_cycle * Vin
integral = 0.0

for i in range(100):
    print(f'循环{i}: vout={vout:.3f}V, duty={duty_cycle:.3f}')
    
    error = Vref - vout
    new_integral = integral + error * dt
    duty_cycle = kp * error + ki * new_integral
    
    if duty_cycle > 1.0:
        duty_cycle = 1.0
    elif duty_cycle < 0.0:
        duty_cycle = 0.0
    else:
        integral = new_integral
    
    vout = duty_cycle * Vin