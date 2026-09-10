import math

class PID_Controller:
    def __init__(self, Kp, Ki, Kd, epsilon):
        self.Kp = Kp
        self.Ki = Ki
        self.Kd = Kd
        self.epsilon = epsilon
        self.last_error = 0.0
        self.integral = 0.0
        self.last_measurement = 0.0

def PID_Calculate(pid, setpoint, measurement):
    error = setpoint - measurement
    
    if abs(error) > pid.epsilon:
        pid.integral = 0.0
    else:
        pid.integral += error
    
    derivative = measurement - pid.last_measurement
    
    output = pid.Kp * error + pid.Ki * pid.integral - pid.Kd * derivative
    
    pid.last_error = error
    pid.last_measurement = measurement
    
    return output

def main():
    pid = PID_Controller(1.2, 0.5, 0.3, 10.0)
    
    test_setpoints = [100.0, 100.0, 100.0, 100.0, 100.0]
    test_measurements = [80.0, 85.0, 95.0, 99.0, 100.5]
    
    print("周期号 | 设定值 | 测量值 | 控制输出")
    print("------------------------------------")
    
    for i in range(5):
        output = PID_Calculate(pid, test_setpoints[i], test_measurements[i])
        print(f"{i+1:6d} | {test_setpoints[i]:6.1f} | {test_measurements[i]:6.1f} | {output:8.2f}")

if __name__ == "__main__":
    main()