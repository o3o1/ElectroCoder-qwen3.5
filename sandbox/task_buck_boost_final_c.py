import math

# 系统参数
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
        self.vout = 0.0        # 输出电压
        self.il = 0.0          # 电感电流
        self.integral_v = 0.0  # 电压环积分项

def voltage_pi_controller(error, dt, integral):
    integral[0] += error * dt
    return KP_V * error + KI_V * integral[0]

def current_p_controller(error):
    return KP_I * error

def simulate_one_step(duty, state):
    # Buck-Boost变换器状态方程
    # dIL/dt = (Vin*duty - |Vout|*(1-duty))/L
    # dVout/dt = (-IL*(1-duty) - Vout/Rload)/C
    
    dIL_dt = (VIN * duty - abs(state.vout) * (1.0 - duty)) / L
    dVout_dt = (-state.il * (1.0 - duty) - state.vout / RLOAD) / C
    
    state.il += dIL_dt * T
    state.vout += dVout_dt * T

def main():
    state = SystemState()
    duty = 0.333  # 初始占空比设为理论值
    
    print("Buck-Boost变换器双环控制仿真")
    print("==============================")
    print(f"输入电压: {VIN:.1f} V")
    print(f"目标输出电压: {VREF:.1f} V")
    print(f"开关频率: {FSW:.0f} Hz")
    print(f"控制周期: {T*1e6:.2f} us")
    print(f"电压环PI: Kp={KP_V:.2f}, Ki={KI_V:.1f}")
    print(f"电流环P: Kp={KP_I:.1f}")
    print(f"负载电阻: {RLOAD:.1f} Ω")
    print()
    
    print("周期序号\tVout(V)\t\tIL(A)\t\t占空比")
    print("-------------------------------------------------")
    
    # 模拟1000个控制周期（10ms实时）
    for i in range(1000):
        # 1. 计算电压误差 → 电压环输出（作为电流参考值）
        v_error = VREF - state.vout
        i_ref = voltage_pi_controller(v_error, T, [state.integral_v])
        
        # 2. 电流限幅保护
        if i_ref > I_MAX:
            i_ref = I_MAX
        if i_ref < -I_MAX:
            i_ref = -I_MAX
        
        # 3. 计算电流误差 → 电流环输出（占空比）
        i_error = i_ref - state.il
        duty_adjust = current_p_controller(i_error)
        duty += duty_adjust * T  # 积分调整占空比
        
        # 4. 占空比限幅0-1
        if duty > 1.0:
            duty = 1.0
        if duty < 0.0:
            duty = 0.0
        
        # 5. 调用simulate_one_step更新电路状态
        simulate_one_step(duty, state)
        
        # 每100个周期打印一次
        if i % 100 == 0:
            print(f"{i}\t\t{state.vout:.3f}\t\t{state.il:.3f}\t\t{duty:.3f}")
    
    print("\n仿真结果：")
    print(f"最终输出电压: {state.vout:.3f} V")
    print(f"最终电感电流: {state.il:.3f} A")
    print(f"最终占空比: {duty:.3f}")
    
    # 理论计算
    theoretical_duty = abs(VREF) / (VIN + abs(VREF))
    theoretical_il = abs(VREF) / RLOAD * (1.0 / (1.0 - theoretical_duty))
    
    print("\n理论值：")
    print(f"理论占空比: {theoretical_duty:.3f}")
    print(f"理论电感电流: {theoretical_il:.3f} A")
    
    # 误差分析
    print("\n误差分析：")
    voltage_error = state.vout - VREF
    voltage_error_percent = abs(voltage_error) / abs(VREF) * 100
    print(f"电压误差: {voltage_error:.3f} V ({voltage_error_percent:.1f}%)")
    
    return 0

if __name__ == "__main__":
    main()