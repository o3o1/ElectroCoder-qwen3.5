import math

class PWM_Control:
    def __init__(self):
        self.PERIOD = 0
        self.DUTY1 = 0
        self.DUTY2 = 0
        self.DEADTIME = 0

def calculate_phase_shift(Vin, Vout, fsw):
    if Vin == 0 or fsw == 0:
        return 0.0
    ratio = Vout / Vin
    if ratio > 1.0:
        ratio = 1.0
    if ratio < -1.0:
        ratio = -1.0
    angle_rad = math.asin(ratio)
    angle_deg = angle_rad * (180.0 / math.pi)
    if angle_deg < 0:
        angle_deg = 0
    if angle_deg > 180:
        angle_deg = 180
    return angle_deg

def optimize_deadtime(phase_shift_deg, fsw, current_direction):
    if fsw == 0:
        return 0.1
    t_dead = 0.1 * (180.0 - phase_shift_deg) / fsw
    if t_dead < 0.1:
        t_dead = 0.1
    return t_dead

def configure_pwm_registers(pwm, fsw, duty1, duty2, deadtime_us):
    pwm.PERIOD = int(1e6 / fsw)
    pwm.DUTY1 = int(pwm.PERIOD * duty1 / 100.0)
    pwm.DUTY2 = int(pwm.PERIOD * duty2 / 100.0)
    pwm.DEADTIME = int(deadtime_us)

def main():
    print("Push-Pull Converter Phase Shift Control Algorithm Simulation")
    print("===========================================================")
    print()
    
    print("Test 1: Vin=48V, Vout=12V, fsw=100kHz")
    Vin = 48.0
    Vout = 12.0
    fsw = 100000.0
    phase_shift = calculate_phase_shift(Vin, Vout, fsw)
    deadtime = optimize_deadtime(phase_shift, fsw, 1)
    print(f"  Phase shift angle: {phase_shift:.2f} degrees")
    print(f"  Optimized deadtime: {deadtime:.2f} us")
    print()
    
    print("Test 2: Vin=24V, Vout=5V, fsw=200kHz")
    Vin = 24.0
    Vout = 5.0
    fsw = 200000.0
    phase_shift = calculate_phase_shift(Vin, Vout, fsw)
    deadtime = optimize_deadtime(phase_shift, fsw, 1)
    print(f"  Phase shift angle: {phase_shift:.2f} degrees")
    print(f"  Optimized deadtime: {deadtime:.2f} us")
    print()
    
    print("Test 3: PWM Register Configuration Simulation")
    pwm_ctrl = PWM_Control()
    configure_pwm_registers(pwm_ctrl, 100000.0, 45.0, 45.0, 0.15)
    print(f"  PERIOD register: {pwm_ctrl.PERIOD}")
    print(f"  DUTY1 register: {pwm_ctrl.DUTY1}")
    print(f"  DUTY2 register: {pwm_ctrl.DUTY2}")
    print(f"  DEADTIME register: {pwm_ctrl.DEADTIME}")
    
    print("\nHardware Register Macro Definition Test:")
    PERIOD_REG = 1000
    DUTY1_REG = 450
    DUTY2_REG = 450
    DEADTIME_REG = 150
    print(f"  PERIOD_REG: {PERIOD_REG}")
    print(f"  DUTY1_REG: {DUTY1_REG}")
    print(f"  DUTY2_REG: {DUTY2_REG}")
    print(f"  DEADTIME_REG: {DEADTIME_REG}")

if __name__ == "__main__":
    main()