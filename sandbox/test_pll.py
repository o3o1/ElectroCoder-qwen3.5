import math
import sys

PI = 3.14159265358979323846
PI_2_3 = 2.0 * PI / 3.0
INTEGRAL_LIMIT = 1000.0

def clark_transform(ua, ub, uc):
    ualpha = (2.0/3.0) * (ua - 0.5 * ub - 0.5 * uc)
    ubeta = (2.0/3.0) * (0.86602540378 * ub - 0.86602540378 * uc)
    return ualpha, ubeta

def park_transform(ualpha, ubeta, theta):
    cos_theta = math.cos(theta)
    sin_theta = math.sin(theta)
    ud = ualpha * cos_theta + ubeta * sin_theta
    uq = -ualpha * sin_theta + ubeta * cos_theta
    return ud, uq

def pi_controller(error, kp, ki, integral, dt):
    integral[0] += error * dt
    if integral[0] > INTEGRAL_LIMIT: integral[0] = INTEGRAL_LIMIT
    if integral[0] < -INTEGRAL_LIMIT: integral[0] = -INTEGRAL_LIMIT
    return kp * error + ki * integral[0]

def pll_update(ua, ub, uc, theta, freq, dt, kp, ki):
    integral = [0.0]
    ualpha, ubeta = clark_transform(ua, ub, uc)
    ud, uq = park_transform(ualpha, ubeta, theta[0])
    
    delta_freq = pi_controller(-uq, kp, ki, integral, dt)
    freq[0] = 50.0 + delta_freq
    theta[0] += 2.0 * PI * freq[0] * dt
    
    if theta[0] > 2.0 * PI: theta[0] -= 2.0 * PI
    if theta[0] < 0: theta[0] += 2.0 * PI

def main():
    dt = 0.001
    kp = 100.0
    ki = 1000.0
    theta_est = [0.0]
    freq_est = [50.0]
    theta_grid = 0.0
    
    for i in range(5):
        ua = math.sin(theta_grid)
        ub = math.sin(theta_grid - PI_2_3)
        uc = math.sin(theta_grid + PI_2_3)
        
        pll_update(ua, ub, uc, theta_est, freq_est, dt, kp, ki)
        
        print(f'Step {i+1}: theta_est = {theta_est[0]:.3f}, theta_real = {theta_grid:.3f}')
        
        theta_grid += 2.0 * PI * 50.0 * dt
        if theta_grid > 2.0 * PI: theta_grid -= 2.0 * PI

if __name__ == '__main__':
    main()