import math

EPSILON = 0.01
BATTERY_VOLTAGE_NOMINAL = 12.0
BATTERY_VOLTAGE_MIN = 10.5
BATTERY_VOLTAGE_MAX = 14.5
BATTERY_VOLTAGE_FLOAT = 13.8
BATTERY_VOLTAGE_CUTOFF = 11.0
BATTERY_VOLTAGE_RESTORE = 11.5
BATTERY_CAPACITY = 7.0
PWM_MAX = 1.0
PWM_MIN = 0.0
PWM_STEP = 0.05

class SolarControllerState:
    def __init__(self):
        self.V_pv = 0.0
        self.I_pv = 0.0
        self.P_pv = 0.0
        self.D = 0.0
        self.V_bat = 0.0
        self.charging_state = 0
        self.load_state = 0

def calculate_pv_current(V_pv, k):
    if V_pv < 0 or V_pv > 24.0:
        return 0.0
    return k * V_pv * (1.0 - V_pv / 24.0)

def mppt_incremental_conductance(state, prev_state, k):
    delta_P = state.P_pv - prev_state.P_pv
    delta_V = state.V_pv - prev_state.V_pv
    
    if abs(delta_V) < EPSILON:
        if delta_P > 0:
            state.D += PWM_STEP
        else:
            state.D -= PWM_STEP
    else:
        dP_dV = delta_P / delta_V
        P_V_ratio = -state.P_pv / state.V_pv
        
        if dP_dV > P_V_ratio:
            state.D += PWM_STEP
        else:
            state.D -= PWM_STEP
    
    if state.D > PWM_MAX:
        state.D = PWM_MAX
    if state.D < PWM_MIN:
        state.D = PWM_MIN
    
    state.V_pv = 24.0 * state.D
    state.I_pv = calculate_pv_current(state.V_pv, k)
    state.P_pv = state.V_pv * state.I_pv

def update_battery_state(state, delta_time):
    charge_current = 0.0
    load_current = 0.0
    
    if state.P_pv > 0 and state.V_bat < BATTERY_VOLTAGE_MAX:
        if state.V_bat < BATTERY_VOLTAGE_FLOAT:
            charge_current = 1.0
            state.charging_state = 1
        else:
            charge_current = 0.1
            state.charging_state = 2
    else:
        state.charging_state = 0
    
    if state.V_bat < BATTERY_VOLTAGE_CUTOFF:
        state.load_state = 0
        load_current = 0.0
    elif state.V_bat >= BATTERY_VOLTAGE_RESTORE:
        state.load_state = 1
        load_current = 0.5
    
    net_current = charge_current - load_current
    delta_V = net_current * delta_time / BATTERY_CAPACITY
    state.V_bat += delta_V
    
    if state.V_bat < BATTERY_VOLTAGE_MIN:
        state.V_bat = BATTERY_VOLTAGE_MIN
    if state.V_bat > BATTERY_VOLTAGE_MAX:
        state.V_bat = BATTERY_VOLTAGE_MAX

def print_state(iteration, state, k):
    charging_text = {0: "充电:关", 1: "充电:恒流", 2: "充电:浮充"}
    load_text = "开" if state.load_state else "关"
    
    print(f"光照k={k:.1f}, 迭代{iteration}: V_pv={state.V_pv:.2f}V, I_pv={state.I_pv:.2f}A, P_pv={state.P_pv:.2f}W, D={state.D:.2f}, V_bat={state.V_bat:.2f}V, {charging_text[state.charging_state]}, 负载:{load_text}")

def main():
    k_values = [0.3, 0.7, 0.9, 0.2]
    iterations_per_k = 5
    
    current_state = SolarControllerState()
    prev_state = SolarControllerState()
    
    current_state.V_bat = 12.0
    current_state.D = 0.5
    current_state.charging_state = 0
    current_state.load_state = 1
    
    print("=== 太阳能路灯控制器模拟 ===")
    
    for k_idx, k in enumerate(k_values):
        print(f"\n--- 光照强度 k={k:.1f} ---")
        
        current_state.V_pv = 24.0 * current_state.D
        current_state.I_pv = calculate_pv_current(current_state.V_pv, k)
        current_state.P_pv = current_state.V_pv * current_state.I_pv
        
        for iter in range(iterations_per_k):
            prev_state.__dict__ = current_state.__dict__.copy()
            
            if iter > 0:
                mppt_incremental_conductance(current_state, prev_state, k)
            
            update_battery_state(current_state, 1.0)
            print_state(iter + 1, current_state, k)
    
    print("\n=== 电池状态总结 ===")
    print(f"最终电池电压: {current_state.V_bat:.2f}V")
    
    charging_text = {0: "未充电", 1: "恒流充电", 2: "浮充"}
    print(f"充电状态: {charging_text[current_state.charging_state]}")
    print(f"负载状态: {'开启' if current_state.load_state else '关闭'}")

if __name__ == "__main__":
    main()