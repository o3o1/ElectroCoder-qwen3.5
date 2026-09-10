class PWM_Monitor:
    def __init__(self, period_ms):
        self.period_ms = period_ms
        self.last_edge_time = 0
        self.missing_count = 0
        self.fault_flag = 0

def pwm_monitor_init(mon, period_ms):
    mon.period_ms = period_ms
    mon.last_edge_time = 0
    mon.missing_count = 0
    mon.fault_flag = 0

def pwm_edge_detected(mon, current_time):
    mon.last_edge_time = current_time
    mon.missing_count = 0

def check_pwm_missing(mon, current_time):
    if mon.fault_flag:
        return 1
    
    if mon.last_edge_time == 0:
        return 0
    
    time_since_last_edge = current_time - mon.last_edge_time
    
    if time_since_last_edge > (mon.period_ms * 1.5):
        mon.missing_count += 1
        
        if mon.missing_count >= 3:
            mon.fault_flag = 1
        
        return 1
    
    return 0

def main():
    monitor = PWM_Monitor(20)
    pwm_monitor_init(monitor, 20)
    
    edge_times = [5, 25, 45, 65, 85]
    edge_count = 5
    edge_index = 0
    
    for time_ms in range(0, 101):
        if edge_index < edge_count and time_ms == edge_times[edge_index]:
            pwm_edge_detected(monitor, time_ms)
            edge_index += 1
        
        missing = check_pwm_missing(monitor, time_ms)
        
        if missing:
            print(f"Time {time_ms}ms: Missing detected! Count={monitor.missing_count}", end="")
            if monitor.fault_flag:
                print(" FAULT!", end="")
            print()
        else:
            print(f"Time {time_ms}ms: No missing")

if __name__ == "__main__":
    main()