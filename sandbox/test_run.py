import math

MAX_FREQUENCY = 50.0
MIN_FREQUENCY = 30.0

def calculate_optimal_frequency(load_rate):
    if load_rate < 0.0:
        load_rate = 0.0
    if load_rate > 1.0:
        load_rate = 1.0
    
    if load_rate < 0.3:
        return 30.0
    elif load_rate < 0.7:
        return 30.0 + (load_rate - 0.3) * 25.0
    else:
        return 50.0

def adjust_frequency(current_freq, target_freq, max_change_rate):
    if max_change_rate <= 0.0:
        return current_freq
    
    diff = target_freq - current_freq
    
    if abs(diff) <= max_change_rate:
        return target_freq
    else:
        if diff > 0:
            return current_freq + max_change_rate
        else:
            return current_freq - max_change_rate

def simulate_adjustment(load_rate, current_freq, max_change_rate):
    print(f"Test case: load_rate={load_rate:.1f}, current_freq={current_freq:.1f}Hz, max_change_rate={max_change_rate:.1f}Hz/cycle")
    
    target_freq = calculate_optimal_frequency(load_rate)
    print(f"  Calculated target frequency: {target_freq:.1f}Hz")
    
    print("  Adjustment process:")
    freq = current_freq
    step = 0
    
    while abs(freq - target_freq) > 0.01:
        freq = adjust_frequency(freq, target_freq, max_change_rate)
        step += 1
        print(f"    Cycle {step}: frequency={freq:.1f}Hz")
    
    print(f"  Final frequency: {freq:.1f}Hz\n")

def main():
    print("=== Oil Pump Variable Frequency Speed Control Energy Saving Algorithm Simulation ===\n")
    
    simulate_adjustment(0.2, 40.0, 5.0)
    simulate_adjustment(0.5, 35.0, 8.0)
    simulate_adjustment(0.8, 45.0, 10.0)

if __name__ == "__main__":
    main()