def calculate_duty_cycle(vin, vout_target):
    if vout_target <= vin:
        return 0.0
    d = 1.0 - (vin / vout_target)
    if d > 0.95:
        return 0.95
    if d < 0.0:
        return 0.0
    return d

def boost_control(vin_measured, vout_measured, vout_target, kp):
    error = vout_target - vout_measured
    delta_d = kp * error
    base_d = calculate_duty_cycle(vin_measured, vout_target)
    d = base_d + delta_d
    
    if d > 0.95:
        return 0.95
    if d < 0.05:
        return 0.05
    return d

print("Boost converter digital control algorithm test")
print("==============================================")
print()

print("Test a) calculate_duty_cycle test:")
vin1 = 12.0
vout_target1 = 24.0
d1 = calculate_duty_cycle(vin1, vout_target1)
print(f"Vin={vin1:.2f}V, Vout_target={vout_target1:.2f}V")
print(f"Calculated duty cycle D = {d1:.4f} ({d1*100:.2f}%)")
print(f"Verification: Vout = Vin/(1-D) = {vin1:.2f}/(1-{d1:.4f}) = {vin1/(1.0-d1):.2f}V")
print()

print("Test b) boost_control test:")

print("Scene 1: Vin=12V, Vout_measured=22V, Vout_target=24V, kp=0.01")
d_scene1 = boost_control(12.0, 22.0, 24.0, 0.01)
print(f"Calculated duty cycle D = {d_scene1:.4f} ({d_scene1*100:.2f}%)")

print("\nScene 2: Vin=12V, Vout_measured=26V, Vout_target=24V, kp=0.01")
d_scene2 = boost_control(12.0, 26.0, 24.0, 0.01)
print(f"Calculated duty cycle D = {d_scene2:.4f} ({d_scene2*100:.2f}%)")

print("\nScene 3: Vin=12V, Vout_measured=15V, Vout_target=24V, kp=0.02")
d_scene3 = boost_control(12.0, 15.0, 24.0, 0.02)
print(f"Calculated duty cycle D = {d_scene3:.4f} ({d_scene3*100:.2f}%)")