import math

VIN = 12.0
VOUT_REF = 24.0
L = 100e-6
C = 470e-6
R = 10.0
FSW = 50000.0
TS = 1.0/FSW

KP = 0.01
KI = 0.5
KD = 0.0001

DUTY_MIN = 0.1
DUTY_MAX = 0.9

iL = 0.0
vout = 0.0
duty = 0.5
e_integral = 0.0
e_prev = 0.0

total_periods = 1000
print_interval = 100

for period in range(1, total_periods + 1):
    error = VOUT_REF - vout
    
    e_integral += error * TS
    e_derivative = (error - e_prev) / TS
    
    duty = KP * error + KI * e_integral + KD * e_derivative
    
    if duty < DUTY_MIN: duty = DUTY_MIN
    if duty > DUTY_MAX: duty = DUTY_MAX
    
    iL = iL + (VIN - (1.0 - duty) * vout) * TS / L
    vout = vout + ((1.0 - duty) * iL - vout / R) * TS / C
    
    e_prev = error
    
    if period % print_interval == 0:
        print(f'Period {period}: Vout={vout:.2f}V, Duty={duty:.3f}, Error={error:.2f}V')

steady_state_error_percent = abs((VOUT_REF - vout) / VOUT_REF) * 100.0
print(f'Final Vout={vout:.2f}V, Steady-state error={steady_state_error_percent:.2f}%')