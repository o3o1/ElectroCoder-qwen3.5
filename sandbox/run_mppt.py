# Python模拟C代码逻辑
DELTA_V_STEP = 0.5
VREF_INIT = 15.0

P_prev = 0.0
Vref_prev = VREF_INIT
first_call = True

def mppt_perturb_and_observe(V, I, Vref):
    global P_prev, Vref_prev, first_call
    P_now = V * I
    
    if first_call:
        P_prev = P_now
        Vref_prev = Vref[0]
        first_call = False
        return P_now
    
    delta_P = P_now - P_prev
    delta_V = Vref[0] - Vref_prev
    
    if delta_P > 0:
        Vref_new = Vref[0] + (DELTA_V_STEP if delta_V > 0 else -DELTA_V_STEP)
    elif delta_P < 0:
        Vref_new = Vref[0] - (DELTA_V_STEP if delta_V > 0 else -DELTA_V_STEP)
    else:
        Vref_new = Vref[0]
    
    P_prev = P_now
    Vref_prev = Vref[0]
    Vref[0] = Vref_new
    
    return P_now

def pv_characteristic(V):
    return -0.1 * (V - 20.0) * (V - 20.0) + 40.0

print("MPPT Perturb and Observe Algorithm Test")
print("=======================================")

Vref = [VREF_INIT]

for i in range(21):
    V = 10.0 + i * 1.0
    P = pv_characteristic(V)
    I = P / V
    
    P_calc = mppt_perturb_and_observe(V, I, Vref)
    
    print(f"V={V:.1f}, I={I:.2f}, P={P_calc:.2f}, Vref={Vref[0]:.2f}")