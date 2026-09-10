import math

VIN = 24.0
VREF = -12.0
I_MAX = 5.0
FSW = 100000.0
T = 1.0/FSW
KP_V = 0.05
KI_V = 2.0
KP_I = 0.3
RLOAD = 6.0
L = 100e-6
C = 470e-6

class SystemState:
    def __init__(self):
        self.vout = 0.0
        self.il = 0.0
        self.integral_v = 0.0

def voltage_pi_controller(error, dt, integral):
    integral[0] += error * dt
    return KP_V * error + KI_V * integral[0]

def current_p_controller(error):
    return KP_I * error

def simulate_one_step(duty, state):
    # Buck-Boost变换器状态方程
    # dIL/dt = (Vin*duty - abs(Vout)*(1-duty))/L
    # dVout/dt = (-IL*(1-duty) - Vout/Rload)/C
    
    dIL_dt = (VIN * duty - abs(state.vout) * (1.0 - duty)) / L
    dVout_dt = (-state.il * (1.0 - duty) - state.vout / RLOAD) / C
    
    state.il += dIL_dt * T
    state.vout += dVout_dt * T

def main():
    state = SystemState()
    duty = 0.0
    
    print("周期序号\tVout(V)\t\tIL(A)\t\t占空比")
    print("-------------------------------------------------")
    
    for i in range(5000):  # 增加仿真时间到50ms
        v_error = VREF - state.vout
        i_ref = voltage_pi_controller(v_error, T, [state.integral_v])
        
        if i_ref > I_MAX:
            i_ref = I_MAX
        if i_ref < -I_MAX:
            i_ref = -I_MAX
        
        i_error = i_ref - state.il
        duty = current_p_controller(i_error)
        
        if duty > 0.95:
            duty = 0.95
        if duty < 0.05:
            duty = 0.05
        
        simulate_one_step(duty, state)
        
        if i % 500 == 0:
            print(f"{i}\t\t{state.vout:.3f}\t\t{state.il:.3f}\t\t{duty:.3f}")
    
    print("\n最终状态：")
    print(f"Vout = {state.vout:.3f} V (目标: {VREF:.1f} V)")
    print(f"IL = {state.il:.3f} A")
    print(f"占空比 = {duty:.3f}")
    
    # 计算理论占空比
    theoretical_duty = abs(VREF) / (VIN + abs(VREF))
    print(f"\n理论占空比: {theoretical_duty:.3f}")

if __name__ == "__main__":
    main()