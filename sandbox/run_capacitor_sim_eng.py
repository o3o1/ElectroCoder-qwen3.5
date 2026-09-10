#!/usr/bin/env python3
# Simulate C program execution

class Capacitor:
    def __init__(self, id, voltage, threshold, alarm_flag=0):
        self.id = id
        self.voltage = voltage
        self.threshold = threshold
        self.alarm_flag = alarm_flag

def check_voltage_balance(caps, tolerance):
    if not caps:
        return
    
    sum_voltage = sum(cap.voltage for cap in caps)
    avg_voltage = sum_voltage / len(caps)
    
    print(f"Average voltage: {avg_voltage:.1f}V")
    
    for cap in caps:
        deviation = ((cap.voltage - avg_voltage) / avg_voltage) * 100.0
        if abs(deviation) > tolerance:
            print(f"Voltage balance check: Capacitor{cap.id} voltage deviation too large ({deviation:.1f}%)")

def detect_overvoltage(caps):
    for cap in caps:
        if cap.voltage > cap.threshold:
            cap.alarm_flag = 1
            print(f"Capacitor{cap.id}: voltage={cap.voltage:.1f}V, status=OVERVOLTAGE ALARM")

def protection_action(caps):
    for cap in caps:
        if cap.alarm_flag == 1:
            cap.voltage = cap.threshold * 0.95
            cap.alarm_flag = 0
    print("Protection action completed")

def main():
    caps = [
        Capacitor(1, 105.0, 110.0),
        Capacitor(2, 112.5, 110.0),
        Capacitor(3, 108.0, 115.0),
        Capacitor(4, 102.0, 110.0),
        Capacitor(5, 107.5, 110.0)
    ]
    
    print("Initial status:")
    for cap in caps:
        print(f"Capacitor{cap.id}: voltage={cap.voltage:.1f}V, threshold={cap.threshold:.1f}V")
    
    print("\nVoltage balance check:")
    check_voltage_balance(caps, 5.0)
    
    print("\nOvervoltage detection:")
    detect_overvoltage(caps)
    
    print("\nProtection action:")
    protection_action(caps)
    
    print("\nFinal status:")
    for cap in caps:
        status = "OVERVOLTAGE ALARM" if cap.alarm_flag == 1 else "NORMAL"
        print(f"Capacitor{cap.id}: voltage={cap.voltage:.1f}V, status={status}")

if __name__ == "__main__":
    main()