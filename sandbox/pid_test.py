def incremental_pid(setpoint, measured, kp, ki, kd):
    global e_k_1, e_k_2
    try:
        e_k_1
    except:
        e_k_1 = 0.0
        e_k_2 = 0.0
    
    e_k = setpoint - measured
    
    delta_u = kp * (e_k - e_k_1) + ki * e_k + kd * (e_k - 2.0 * e_k_1 + e_k_2)
    
    e_k_2 = e_k_1
    e_k_1 = e_k
    
    return delta_u

kp = 1.2
ki = 0.5
kd = 0.1
setpoint = 10.0

measurements = [10.0, 9.5, 9.0, 8.8, 8.5]

for i, measured in enumerate(measurements):
    error = setpoint - measured
    delta_u = incremental_pid(setpoint, measured, kp, ki, kd)
    print('周期{}: 测量值={:.3f}, 误差={:.3f}, Δu={:.3f}'.format(i+1, measured, error, delta_u))