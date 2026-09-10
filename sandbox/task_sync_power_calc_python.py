import math

PI = 3.14159265358979323846
SAMPLE_RATE = 10000.0
GRID_FREQ = 50.0
GRID_VOLTAGE = 220.0
SAMPLE_PERIOD = 1.0 / SAMPLE_RATE
OMEGA_NOMINAL = 2.0 * PI * GRID_FREQ

class PLLessSync:
    def __init__(self):
        self.theta = 0.0
        self.freq = GRID_FREQ
        self.u_alpha_prev = 0.0
        self.u_beta_prev = 0.0
        self.integral = 0.0
        self.kp = 100.0
        self.ki = 5000.0

def clarke_transform(ua, ub, uc):
    u_alpha = (2.0/3.0) * (ua - 0.5*ub - 0.5*uc)
    u_beta = (2.0/3.0) * (0.86602540378*ub - 0.86602540378*uc)
    return u_alpha, u_beta

def plless_sync_update(sync, ua, ub, uc):
    u_alpha, u_beta = clarke_transform(ua, ub, uc)
    
    q = u_alpha * sync.u_beta_prev - u_beta * sync.u_alpha_prev
    u_sq = u_alpha*u_alpha + u_beta*u_beta
    
    phase_error = 0.0
    if u_sq > 1e-6:
        phase_error = q / u_sq
    
    delta_theta = sync.kp * phase_error + sync.ki * sync.integral
    sync.integral += phase_error * SAMPLE_PERIOD
    sync.theta += (OMEGA_NOMINAL + delta_theta) * SAMPLE_PERIOD
    
    while sync.theta > PI:
        sync.theta -= 2.0 * PI
    while sync.theta < -PI:
        sync.theta += 2.0 * PI
    
    sync.freq = GRID_FREQ + delta_theta / (2.0 * PI)
    sync.u_alpha_prev = u_alpha
    sync.u_beta_prev = u_beta

def generate_three_phase_voltage(time):
    omega = 2.0 * PI * GRID_FREQ
    phase = omega * time
    
    ua = GRID_VOLTAGE * math.sin(phase)
    ub = GRID_VOLTAGE * math.sin(phase - 2.0*PI/3.0)
    uc = GRID_VOLTAGE * math.sin(phase + 2.0*PI/3.0)
    
    return ua, ub, uc

def main():
    sync = PLLessSync()
    
    total_samples = int(5.0 / GRID_FREQ * SAMPLE_RATE)
    time = 0.0
    
    freq_sum = 0.0
    valid_count = 0
    
    print("PLL-less Sync Control Simulation")
    print(f"Sampling Rate: {SAMPLE_RATE:.0f} Hz, Grid Freq: {GRID_FREQ:.1f} Hz, Voltage: {GRID_VOLTAGE:.1f} V")
    print("=" * 60)
    print("Sample\tTime(s)\tPhase(rad)\tFreq(Hz)\tFreqError(Hz)")
    print("=" * 60)
    
    for i in range(total_samples):
        ua, ub, uc = generate_three_phase_voltage(time)
        plless_sync_update(sync, ua, ub, uc)
        freq_error = sync.freq - GRID_FREQ
        
        if i > 100:
            freq_sum += abs(freq_error)
            valid_count += 1
        
        if i % 100 == 0:
            print(f"{i}\t{time:.4f}\t{sync.theta:.4f}\t\t{sync.freq:.4f}\t\t{freq_error:.4f}")
        
        time += SAMPLE_PERIOD
    
    avg_freq_error = freq_sum / valid_count if valid_count > 0 else 0.0
    
    print("=" * 60)
    print(f"Simulation complete! Processed {total_samples} samples ({total_samples * GRID_FREQ / SAMPLE_RATE:.1f} cycles)")
    print(f"Average frequency error: {avg_freq_error:.6f} Hz")
    print(f"Relative error: {avg_freq_error / GRID_FREQ * 100.0:.4f}%")

if __name__ == "__main__":
    main()