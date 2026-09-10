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
    # 导通阶段：电感充电，电容放电
    # 关断阶段：电感放电，电容充电
    dIL_dt = (VIN * duty - abs(state.vout) * (1.0 - duty)) / L
    dVout_dt = (-state.il * (1.0 - duty) - state.vout / RLOAD) / C
    
    state.il += dIL_dt * T
    state.vout += dVout_dt * T

def main():
    state = SystemState()
    duty = 0.0
    
    print("周期序号\tVout(V)\t\tIL(A)\t\t占空比")
    print("-------------------------------------------------")
    
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
        duty = current_p_controller(i_error)
        
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
    
    print("\n最终状态：")
    print(f"Vout = {state.vout:.3f} V (目标: {VREF:.1f} V)")
    print(f"IL = {state.il:.3f} A")
    print(f"占空比 = {duty:.3f}")
    
    # 验证结果
    theoretical_duty = abs(VREF) / (VIN + abs(VREF))
    print(f"理论占空比: {theoretical_duty:.3f}")
    
    # 检查是否接近目标
    if abs(state.vout - VREF) < 1.0:
        print("控制效果：良好")
    else:
        print("控制效果：需要调整参数")

if __name__ == "__main__":
    main()