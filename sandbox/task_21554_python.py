import random
import time

VOLTAGE_THRESHOLD_HIGH = 650
VOLTAGE_THRESHOLD_LOW = 600
VOLTAGE_MIN = 300
VOLTAGE_MAX = 800
PWM_STEP = 10
NUM_CYCLES = 10

current_pwm = 0
random.seed(time.time())

for cycle in range(1, NUM_CYCLES + 1):
    voltage = random.randint(VOLTAGE_MIN, VOLTAGE_MAX)
    
    if voltage > VOLTAGE_THRESHOLD_HIGH:
        if current_pwm < 100:
            current_pwm += PWM_STEP
            if current_pwm > 100:
                current_pwm = 100
    elif voltage < VOLTAGE_THRESHOLD_LOW:
        if current_pwm > 0:
            current_pwm -= PWM_STEP
            if current_pwm < 0:
                current_pwm = 0
    
    state = "ON" if current_pwm > 0 else "OFF"
    print(f"周期{cycle}: 电压={voltage}V, PWM={current_pwm}%, 状态={state}")
    
    time.sleep(0.5)