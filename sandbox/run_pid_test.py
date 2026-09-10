def incremental_pid(target, measure, kp, ki, kd):
    e_k_1 = 0.0
    e_k_2 = 0.0
    
    def pid_step(measure_val):
        nonlocal e_k_1, e_k_2
        e_k = target - measure_val
        delta_u = kp * (e_k - e_k_1) + ki * e_k + kd * (e_k - 2.0 * e_k_1 + e_k_2)
        
        e_k_2 = e_k_1
        e_k_1 = e_k
        
        return delta_u
    
    return pid_step

print("Test case 1: target=10.0, Kp=0.8, Ki=0.2, Kd=0.1")
measurements1 = [8.0, 9.0, 9.5, 10.0]
pid1 = incremental_pid(10.0, 0.0, 0.8, 0.2, 0.1)
for i, measure in enumerate(measurements1):
    delta_u = pid1(measure)
    print(f"  measure={measure:.1f}, delta_u(k)={delta_u:.6f}")

print("\nTest case 2: target=5.0, Kp=1.0, Ki=0.5, Kd=0.2")
measurements2 = [2.0, 3.0, 4.0, 5.0]
pid2 = incremental_pid(5.0, 0.0, 1.0, 0.5, 0.2)
for i, measure in enumerate(measurements2):
    delta_u = pid2(measure)
    print(f"  measure={measure:.1f}, delta_u(k)={delta_u:.6f}")

print("\nTest case 3: target=0.0, Kp=0.5, Ki=0.1, Kd=0.05")
measurements3 = [1.0, 0.5, 0.2, 0.0]
pid3 = incremental_pid(0.0, 0.0, 0.5, 0.1, 0.05)
for i, measure in enumerate(measurements3):
    delta_u = pid3(measure)
    print(f"  measure={measure:.1f}, delta_u(k)={delta_u:.6f}")