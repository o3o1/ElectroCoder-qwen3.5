import math

def calculate_zvs_delay(resonant_inductance, parasitic_capacitance, load_current, bus_voltage):
    Lr = resonant_inductance * 1e-6
    Cr = parasitic_capacitance * 1e-9
    
    resonant_time = (math.pi / 2) * math.sqrt(Lr * Cr)
    current_commutation_time = (2 * Lr * load_current) / bus_voltage
    
    delay_seconds = resonant_time - current_commutation_time
    
    if delay_seconds < 0:
        delay_seconds = 0
    
    return delay_seconds * 1e6

def generate_pwm_signals(zvs_delay_us):
    PWM_PERIOD = 10
    period_us = 20.0
    time_per_point = period_us / PWM_PERIOD
    
    delay_points = int(zvs_delay_us / time_per_point + 0.5)
    if delay_points < 0: delay_points = 0
    if delay_points > PWM_PERIOD: delay_points = PWM_PERIOD
    
    q1_state = []
    q2_state = []
    q3_state = []
    q4_state = []
    
    for i in range(PWM_PERIOD):
        phase = i % (PWM_PERIOD // 2)
        
        q1_state.append(1 if phase < PWM_PERIOD // 4 else 0)
        q2_state.append(1 if phase >= PWM_PERIOD // 4 else 0)
        
        delayed_phase = (i - delay_points + PWM_PERIOD) % PWM_PERIOD
        delayed_mod_phase = delayed_phase % (PWM_PERIOD // 2)
        
        q3_state.append(1 if delayed_mod_phase < PWM_PERIOD // 4 else 0)
        q4_state.append(1 if delayed_mod_phase >= PWM_PERIOD // 4 else 0)
    
    return q1_state, q2_state, q3_state, q4_state

def main():
    Lr = 10.0
    Cr = 5.0
    I_load = 5.0
    V_bus = 400.0
    
    zvs_delay = calculate_zvs_delay(Lr, Cr, I_load, V_bus)
    
    print("ZVS延迟时间: {:.2f} μs".format(zvs_delay))
    
    q1, q2, q3, q4 = generate_pwm_signals(zvs_delay)
    
    print("Q1状态: [{}]".format(",".join(map(str, q1))))
    print("Q2状态: [{}]".format(",".join(map(str, q2))))
    print("Q3状态: [{}]".format(",".join(map(str, q3))))
    print("Q4状态: [{}]".format(",".join(map(str, q4))))

if __name__ == "__main__":
    main()