import time

CURRENT_THRESHOLD = 100.0
mock_pwm_register = 100

def read_collector_current():
    global call_count
    if 'call_count' not in globals():
        call_count = 0
    currents = [80.0, 120.0, 99.9]
    current = currents[call_count % 3]
    call_count += 1
    return current

def soft_shutdown():
    print('Overcurrent detected, soft shutdown initiated')
    
    for duty_cycle in range(100, -1, -10):
        global mock_pwm_register
        mock_pwm_register = duty_cycle
        print(f'PWM duty cycle: {duty_cycle}%')
        time.sleep(0.1)
    
    mock_pwm_register = 0
    print('Soft shutdown completed')

def main():
    print('IGBT Overcurrent Protection Simulation')
    print('======================================\n')
    
    for test_case in range(3):
        print(f'Test Case {test_case + 1}:')
        
        current = read_collector_current()
        print(f'Measured current: {current:.1f}A')
        
        if current >= CURRENT_THRESHOLD:
            soft_shutdown()
        elif current >= CURRENT_THRESHOLD * 0.95:
            print('Current near threshold')
        else:
            print('Current normal')
        
        print()

if __name__ == '__main__':
    main()