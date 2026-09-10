import math

KP = 100.0
KI = 5000.0
NOMINAL_FREQ = 50.0
SAMPLE_RATE = 10000.0
DT = 1.0 / SAMPLE_RATE
PRINT_INTERVAL = 0.01

class PLL_State:
    def __init__(self):
        self.phase = 0.0
        self.freq = 50.0
        self.integral = 0.0

def pll_step(state, voltage_sample, dt):
    error = voltage_sample * math.cos(state.phase)
    
    state.freq = NOMINAL_FREQ + KP * error + KI * state.integral
    
    state.integral += error * dt
    
    state.phase += 2.0 * math.pi * state.freq * dt
    
    state.phase = state.phase % (2.0 * math.pi)
    if state.phase < 0:
        state.phase += 2.0 * math.pi

def main():
    pll = PLL_State()
    
    simulation_time = 0.1
    total_samples = int(simulation_time * SAMPLE_RATE)
    print_interval_samples = int(PRINT_INTERVAL * SAMPLE_RATE)
    
    print("Time: {:.4f}s, Actual: {:.3f} rad, Estimated: {:.3f} rad, Freq: {:.2f} Hz".format(0.0, 0.0, pll.phase, pll.freq))
    
    for i in range(1, total_samples + 1):
        t = i * DT
        actual_phase = 2.0 * math.pi * 50.0 * t
        voltage_sample = math.sin(actual_phase)
        
        pll_step(pll, voltage_sample, DT)
        
        if i % print_interval_samples == 0:
            normalized_actual_phase = actual_phase % (2.0 * math.pi)
            if normalized_actual_phase < 0:
                normalized_actual_phase += 2.0 * math.pi
            print("Time: {:.4f}s, Actual: {:.3f} rad, Estimated: {:.3f} rad, Freq: {:.2f} Hz".format(t, normalized_actual_phase, pll.phase, pll.freq))

if __name__ == "__main__":
    main()