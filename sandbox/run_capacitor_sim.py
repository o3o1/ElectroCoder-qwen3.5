#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 模拟C程序的执行

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
    
    print(f"平均电压: {avg_voltage:.1f}V")
    
    for cap in caps:
        deviation = ((cap.voltage - avg_voltage) / avg_voltage) * 100.0
        if abs(deviation) > tolerance:
            print(f"均压检查: 电容{cap.id}电压偏差过大({deviation:.1f}%)")

def detect_overvoltage(caps):
    for cap in caps:
        if cap.voltage > cap.threshold:
            cap.alarm_flag = 1
            print(f"电容{cap.id}: 电压={cap.voltage:.1f}V, 状态=过压告警")

def protection_action(caps):
    for cap in caps:
        if cap.alarm_flag == 1:
            cap.voltage = cap.threshold * 0.95
            cap.alarm_flag = 0
    print("保护动作执行完成")

def main():
    caps = [
        Capacitor(1, 105.0, 110.0),
        Capacitor(2, 112.5, 110.0),
        Capacitor(3, 108.0, 115.0),
        Capacitor(4, 102.0, 110.0),
        Capacitor(5, 107.5, 110.0)
    ]
    
    print("初始状态:")
    for cap in caps:
        print(f"电容{cap.id}: 电压={cap.voltage:.1f}V, 阈值={cap.threshold:.1f}V")
    
    print("\n均压检查:")
    check_voltage_balance(caps, 5.0)
    
    print("\n过压检测:")
    detect_overvoltage(caps)
    
    print("\n保护动作:")
    protection_action(caps)
    
    print("\n最终状态:")
    for cap in caps:
        status = "过压告警" if cap.alarm_flag == 1 else "正常"
        print(f"电容{cap.id}: 电压={cap.voltage:.1f}V, 状态={status}")

if __name__ == "__main__":
    main()