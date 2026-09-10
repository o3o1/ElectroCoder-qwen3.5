import math

def vf_control(freq_hz, base_freq, base_voltage, torque_boost_percent):
    voltage_amplitude = 0.0
    
    if freq_hz <= base_freq:
        voltage_amplitude = (freq_hz / base_freq) * base_voltage
    else:
        voltage_amplitude = base_voltage
    
    if freq_hz < 10.0:
        boost_factor = 1.0 + (torque_boost_percent / 100.0) * ((10.0 - freq_hz) / 10.0)
        voltage_amplitude *= boost_factor
    
    angle = 0.0
    phase_angle_u = angle
    phase_angle_v = angle - 2.0 * math.pi / 3.0
    phase_angle_w = angle + 2.0 * math.pi / 3.0
    
    voltage_u = voltage_amplitude * math.sin(phase_angle_u)
    voltage_v = voltage_amplitude * math.sin(phase_angle_v)
    voltage_w = voltage_amplitude * math.sin(phase_angle_w)
    
    return voltage_amplitude, voltage_u, voltage_v, voltage_w

print("=== Three-phase induction motor V/F control algorithm test ===\n")

print("Test 1: Normal V/F region")
print("Parameters: freq_hz=30Hz, base_freq=50Hz, base_voltage=220V, torque_boost=0%")
amp, u, v, w = vf_control(30.0, 50.0, 220.0, 0.0)
print(f"Voltage amplitude: {amp:.2f}V")
print(f"U phase voltage: {u:.2f}V")
print(f"V phase voltage: {v:.2f}V")
print(f"W phase voltage: {w:.2f}V")
print()

print("Test 2: Torque boost")
print("Parameters: freq_hz=5Hz, base_freq=50Hz, base_voltage=220V, torque_boost=20%")
amp, u, v, w = vf_control(5.0, 50.0, 220.0, 20.0)
base_voltage_5hz = (5.0/50.0) * 220.0
boost_factor = 1.0 + (20.0/100.0) * ((10.0 - 5.0)/10.0)
print(f"Voltage amplitude: {amp:.2f}V (base: {base_voltage_5hz:.2f}V * boost factor: {boost_factor:.2f})")
print(f"U phase voltage: {u:.2f}V")
print(f"V phase voltage: {v:.2f}V")
print(f"W phase voltage: {w:.2f}V")
print()

print("Test 3: Field weakening region")
print("Parameters: freq_hz=60Hz, base_freq=50Hz, base_voltage=220V, torque_boost=0%")
amp, u, v, w = vf_control(60.0, 50.0, 220.0, 0.0)
print(f"Voltage amplitude: {amp:.2f}V (maintain rated voltage)")
print(f"U phase voltage: {u:.2f}V")
print(f"V phase voltage: {v:.2f}V")
print(f"W phase voltage: {w:.2f}V")