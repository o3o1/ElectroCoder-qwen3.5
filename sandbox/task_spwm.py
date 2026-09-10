import math
import sys

SIN_TABLE_SIZE = 256
CARRIER_FREQ_HZ = 10000.0
CARRIER_PERIOD_US = 1000000.0 / CARRIER_FREQ_HZ

sin_table = [0.5 * (1.0 + math.sin(2.0 * math.pi * i / SIN_TABLE_SIZE)) for i in range(SIN_TABLE_SIZE)]

def generate_spwm_pulses(modulation_index, frequency_hz, cycles):
    if modulation_index < 0.0 or modulation_index > 1.0:
        return
    if frequency_hz <= 0.0:
        return
    if cycles <= 0:
        return
    
    sin_period_us = 1000000.0 / frequency_hz
    sin_step_us = sin_period_us / SIN_TABLE_SIZE
    
    total_steps = cycles * SIN_TABLE_SIZE
    current_time_us = 0.0
    last_state = 0
    
    for step in range(total_steps):
        table_index = step % SIN_TABLE_SIZE
        sin_value = sin_table[table_index]
        modulated_value = sin_value * modulation_index
        
        step_start_time = step * sin_step_us
        step_end_time = (step + 1) * sin_step_us
        
        carrier_cycles = int((step_end_time - step_start_time) / CARRIER_PERIOD_US)
        if carrier_cycles < 1:
            carrier_cycles = 1
        
        carrier_step_us = (step_end_time - step_start_time) / carrier_cycles
        
        for carrier in range(carrier_cycles):
            carrier_start = step_start_time + carrier * carrier_step_us
            carrier_mid = carrier_start + carrier_step_us / 2.0
            
            new_state = 1 if modulated_value > 0.5 else 0
            
            if new_state != last_state:
                print(f'{carrier_mid:.2f}: {new_state}')
                last_state = new_state
    
    if last_state == 1:
        print(f'{total_steps * sin_step_us:.2f}: 0')

print('=== 测试1: 调制比0.5, 频率50Hz, 2个周期 ===')
generate_spwm_pulses(0.5, 50.0, 2)

print('\n=== 测试2: 调制比0.8, 频率60Hz, 1个周期 ===')
generate_spwm_pulses(0.8, 60.0, 1)

print('\n=== 测试3: 调制比1.0, 频率100Hz, 1个周期 ===')
generate_spwm_pulses(1.0, 100.0, 1)