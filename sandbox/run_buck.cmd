@echo off
cd sandbox
python -c "
import math

class BuckController:
    def __init__(self, kp, ki, kd, v_ref):
        self.kp = kp
        self.ki = ki
        self.kd = kd
        self.integral = 0.0
        self.prev_error = 0.0
        self.duty_cycle = 0.5
        self.v_ref = v_ref

def buck_init(ctrl, kp, ki, kd, v_ref):
    ctrl.kp = kp
    ctrl.ki = ki
    ctrl.kd = kd
    ctrl.integral = 0.0
    ctrl.prev_error = 0.0
    ctrl.duty_cycle = 0.5
    ctrl.v_ref = v_ref

def buck_update(ctrl, v_measured, dt):
    error = ctrl.v_ref - v_measured
    
    ctrl.integral += error * dt
    
    derivative = (error - ctrl.prev_error) / dt
    
    output = ctrl.kp * error + ctrl.ki * ctrl.integral + ctrl.kd * derivative
    
    if output > 0.5: output = 0.5
    if output < -0.5: output = -0.5
    
    ctrl.duty_cycle += output
    
    if ctrl.duty_cycle > 1.0: ctrl.duty_cycle = 1.0
    if ctrl.duty_cycle < 0.0: ctrl.duty_cycle = 0.0
    
    ctrl.prev_error = error

ctrl = BuckController(0, 0, 0, 0)
buck_init(ctrl, 0.1, 0.05, 0.01, 5.0)

voltages = [4.5, 4.8, 5.0, 5.1, 5.05]
dt = 0.001

for i in range(5):
    buck_update(ctrl, voltages[i], dt)
    print('Period %d: V=%.3f, duty=%.3f' % (i+1, voltages[i], ctrl.duty_cycle))
"