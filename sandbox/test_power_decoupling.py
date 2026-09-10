import math

L = 0.001
OMEGA = 314.16
KP = 10.0
KI = 100.0
DT = 0.0001

integral_d = 0.0
integral_q = 0.0

def power_decoupling(v_d, v_q, i_d_ref, i_q_ref):
    global integral_d, integral_q
    
    i_d_meas = 0.0
    i_q_meas = 0.0
    
    error_d = i_d_ref - i_d_meas
    error_q = i_q_ref - i_q_meas
    
    integral_d += error_d * DT
    integral_q += error_q * DT
    
    u_d = v_d - OMEGA * L * i_q_ref + KP * error_d + KI * integral_d
    u_q = v_q + OMEGA * L * i_d_ref + KP * error_q + KI * integral_q
    
    return u_d, u_q

print("Active/Reactive Power Decoupling Control Algorithm Test")
print("=======================================================\n")

print("1. Active power control test (i_d_ref = 100.0, i_q_ref = 0.0):")
u_d, u_q = power_decoupling(311.0, 0.0, 100.0, 0.0)
print(f"   u_d = {u_d:.6f} V")
print(f"   u_q = {u_q:.6f} V\n")

print("2. Reactive power control test (i_d_ref = 0.0, i_q_ref = 50.0):")
u_d, u_q = power_decoupling(311.0, 0.0, 0.0, 50.0)
print(f"   u_d = {u_d:.6f} V")
print(f"   u_q = {u_q:.6f} V\n")

print("3. Combined control test (i_d_ref = 80.0, i_q_ref = 30.0):")
u_d, u_q = power_decoupling(311.0, 0.0, 80.0, 30.0)
print(f"   u_d = {u_d:.6f} V")
print(f"   u_q = {u_q:.6f} V\n")

print("4. Second call to verify integral accumulation:")
u_d, u_q = power_decoupling(311.0, 0.0, 80.0, 30.0)
print(f"   u_d = {u_d:.6f} V")
print(f"   u_q = {u_q:.6f} V")