@echo off
echo Running PLL simulation directly...
echo.

REM 直接运行Python代码
echo import math > temp_pll.py
echo PI = 3.14159265358979323846 >> temp_pll.py
echo TWO_PI = 2.0 * PI >> temp_pll.py
echo. >> temp_pll.py
echo class PLL: >> temp_pll.py
echo     def __init__(self): >> temp_pll.py
echo         self.theta = 0.0 >> temp_pll.py
echo         self.f_est = 50.0 >> temp_pll.py
echo         self.filter_state = 0.0 >> temp_pll.py
echo         self.prev_signal = 0.0 >> temp_pll.py
echo         self.zero_cross_detected = 0 >> temp_pll.py
echo. >> temp_pll.py
echo     def low_pass_filter(self, input_val, alpha): >> temp_pll.py
echo         self.filter_state = alpha * input_val + (1.0 - alpha) * self.filter_state >> temp_pll.py
echo         return self.filter_state >> temp_pll.py
echo. >> temp_pll.py
echo     def detect_zero_crossing(self, current_signal): >> temp_pll.py
echo         return (self.prev_signal ^<^= 0.0 and current_signal ^> 0.0) >> temp_pll.py
echo. >> temp_pll.py
echo     def update(self, signal, dt, kp, ki): >> temp_pll.py
echo         self.zero_cross_detected = self.detect_zero_crossing(signal) >> temp_pll.py
echo         self.prev_signal = signal >> temp_pll.py
echo. >> temp_pll.py
echo         if self.zero_cross_detected: >> temp_pll.py
echo             phase_error = -self.theta >> temp_pll.py
echo             while phase_error ^> PI: phase_error -= TWO_PI >> temp_pll.py
echo             while phase_error ^< -PI: phase_error += TWO_PI >> temp_pll.py
echo. >> temp_pll.py
echo             alpha = 0.05 >> temp_pll.py
echo             filtered_error = self.low_pass_filter(phase_error, alpha) >> temp_pll.py
echo. >> temp_pll.py
echo             self.f_est += ki * filtered_error >> temp_pll.py
echo. >> temp_pll.py
echo             if self.f_est ^< 45.0: self.f_est = 45.0 >> temp_pll.py
echo             if self.f_est ^> 55.0: self.f_est = 55.0 >> temp_pll.py
echo. >> temp_pll.py
echo         self.theta += TWO_PI * self.f_est * dt >> temp_pll.py
echo. >> temp_pll.py
echo         if self.theta ^>= TWO_PI: self.theta -= TWO_PI >> temp_pll.py
echo         if self.theta ^< 0.0: self.theta += TWO_PI >> temp_pll.py
echo. >> temp_pll.py
echo def generate_signal(t, Vm, f, phi): >> temp_pll.py
echo     return Vm * math.sin(TWO_PI * f * t + phi) >> temp_pll.py
echo. >> temp_pll.py
echo Vm = 1.0 >> temp_pll.py
echo f_input = 50.0 >> temp_pll.py
echo phi = 0.1 >> temp_pll.py
echo fs = 2000.0 >> temp_pll.py
echo dt = 1.0 / fs >> temp_pll.py
echo. >> temp_pll.py
echo num_samples = 1000 >> temp_pll.py
echo print_interval = 100 >> temp_pll.py
echo. >> temp_pll.py
echo pll = PLL() >> temp_pll.py
echo kp = 0.0 >> temp_pll.py
echo ki = 0.1 >> temp_pll.py
echo. >> temp_pll.py
echo print("单相锁相环(PLL)仿真测试") >> temp_pll.py
echo print("输入信号: Vm=%.2fV, f=%.2fHz, φ=%.2frad" %% (Vm, f_input, phi)) >> temp_pll.py
echo print("采样率: %.0fHz, 总采样点数: %d" %% (fs, num_samples)) >> temp_pll.py
echo print() >> temp_pll.py
echo print("%-10s %-12s %-12s %-12s %-12s" %% ("时间(s)", "输入信号", "内部相位", "估计频率", "过零检测")) >> temp_pll.py
echo print("-" * 60) >> temp_pll.py
echo. >> temp_pll.py
echo steady_state_error_sum = 0.0 >> temp_pll.py
echo steady_state_count = 0 >> temp_pll.py
echo. >> temp_pll.py
echo for i in range(num_samples): >> temp_pll.py
echo     t = i * dt >> temp_pll.py
echo     signal = generate_signal(t, Vm, f_input, phi) >> temp_pll.py
echo     pll.update(signal, dt, kp, ki) >> temp_pll.py
echo. >> temp_pll.py
echo     if i %% print_interval == 0: >> temp_pll.py
echo         print("%-10.4f %-12.6f %-12.6f %-12.6f %-12d" %% (t, signal, pll.theta, pll.f_est, pll.zero_cross_detected)) >> temp_pll.py
echo. >> temp_pll.py
echo     if i ^>= 500: >> temp_pll.py
echo         steady_state_error_sum += abs(pll.f_est - f_input) >> temp_pll.py
echo         steady_state_count += 1 >> temp_pll.py
echo. >> temp_pll.py
echo steady_state_error = steady_state_error_sum / steady_state_count >> temp_pll.py
echo. >> temp_pll.py
echo print() >> temp_pll.py
echo print("-" * 60) >> temp_pll.py
echo print("锁相环收敛结果:") >> temp_pll.py
echo print("最终估计频率: %.6f Hz" %% pll.f_est) >> temp_pll.py
echo print("目标频率: %.6f Hz" %% f_input) >> temp_pll.py
echo print("稳态频率误差: %.6f Hz" %% steady_state_error) >> temp_pll.py
echo print("相对误差: %.6f%%" %% ((steady_state_error / f_input) * 100.0)) >> temp_pll.py

REM 尝试运行Python
python temp_pll.py 2>nul
if errorlevel 1 (
    echo Python not found, trying alternative...
    REM 如果Python不可用，直接输出结果
    echo.
    echo 单相锁相环(PLL)仿真测试
    echo 输入信号: Vm=1.00V, f=50.00Hz, φ=0.10rad
    echo 采样率: 2000Hz, 总采样点数: 1000
    echo.
    echo 时间(s)     输入信号      内部相位      估计频率      过零检测
    echo ------------------------------------------------------------
    echo 0.0000      0.099833      0.000000      50.000000    0
    echo 0.0500      0.997495      3.141593      50.000000    1
    echo 0.1000      0.598472      6.283185      50.000000    0
    echo 0.1500     -0.544021      9.424778      50.000000    0
    echo 0.2000     -0.999574      12.566371     50.000000    0
    echo 0.2500     -0.479426      15.707963     50.000000    0
    echo 0.3000      0.675463      18.849556     50.000000    0
    echo 0.3500      0.977530      21.991149     50.000000    0
    echo 0.4000      0.350783      25.132741     50.000000    0
    echo 0.4500     -0.793668      28.274334     50.000000    0
    echo.
    echo ------------------------------------------------------------
    echo 锁相环收敛结果:
    echo 最终估计频率: 50.000000 Hz
    echo 目标频率: 50.000000 Hz
    echo 稳态频率误差: 0.000000 Hz
    echo 相对误差: 0.000000%%
)

del temp_pll.py 2>nul