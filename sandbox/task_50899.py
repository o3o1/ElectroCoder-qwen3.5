import ctypes
import sys

PWM_PERIOD = 1000
DEAD_TIME = 50

class Registers:
    def __init__(self):
        self.reg1 = ctypes.c_uint32(0)
        self.reg2 = ctypes.c_uint32(0)
        self.reg3 = ctypes.c_uint32(0)
        self.reg4 = ctypes.c_uint32(0)
        
        self.PWM1_H = ctypes.pointer(self.reg1)
        self.PWM1_L = ctypes.pointer(self.reg2)
        self.PWM2_H = ctypes.pointer(self.reg3)
        self.PWM2_L = ctypes.pointer(self.reg4)

def phase_shift_pwm(regs, duty1, duty2, phase_shift):
    if duty1 > PWM_PERIOD:
        duty1 = PWM_PERIOD
    if duty2 > PWM_PERIOD:
        duty2 = PWM_PERIOD
    if phase_shift > PWM_PERIOD:
        phase_shift = phase_shift % PWM_PERIOD
    
    regs.PWM1_H.contents.value = duty1
    regs.PWM1_L.contents.value = (duty1 + DEAD_TIME) % PWM_PERIOD
    regs.PWM2_H.contents.value = (duty2 + phase_shift) % PWM_PERIOD
    regs.PWM2_L.contents.value = (duty2 + phase_shift + DEAD_TIME) % PWM_PERIOD

def main():
    regs = Registers()
    
    phase_shift_pwm(regs, 600, 600, 200)
    print(f"Test 1: PWM1_H={regs.reg1.value}, PWM1_L={regs.reg2.value}, PWM2_H={regs.reg3.value}, PWM2_L={regs.reg4.value}")
    
    phase_shift_pwm(regs, 300, 700, 500)
    print(f"Test 2: PWM1_H={regs.reg1.value}, PWM1_L={regs.reg2.value}, PWM2_H={regs.reg3.value}, PWM2_L={regs.reg4.value}")
    
    phase_shift_pwm(regs, 900, 400, 800)
    print(f"Test 3: PWM1_H={regs.reg1.value}, PWM1_L={regs.reg2.value}, PWM2_H={regs.reg3.value}, PWM2_L={regs.reg4.value}")

if __name__ == "__main__":
    main()