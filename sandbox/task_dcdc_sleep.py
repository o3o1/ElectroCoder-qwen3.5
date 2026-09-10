#!/usr/bin/env python3
"""
DC/DC变换器休眠唤醒控制模拟程序
"""

import sys

# 模拟硬件寄存器宏定义
SLEEP_THRESHOLD = 15   # 休眠阈值(mA)
WAKE_THRESHOLD = 25    # 唤醒阈值(mA)

# 状态定义
STATE_ACTIVE = 0
STATE_SLEEP = 1

# 全局状态变量
current_state = STATE_ACTIVE
transition_count = 0

# 模拟寄存器
load_current_reg = 0
control_reg = 0

def mock_set_load_current(current):
    """模拟设置负载电流寄存器"""
    global load_current_reg
    load_current_reg = current

def mock_get_load_current():
    """模拟读取负载电流寄存器"""
    return load_current_reg

def mock_set_control_reg(value):
    """模拟设置控制寄存器"""
    global control_reg
    control_reg = value

def dcdc_state_machine(load_current):
    """DC/DC状态机函数"""
    global current_state, transition_count
    
    # 设置负载电流到模拟寄存器
    mock_set_load_current(load_current)
    
    new_state = current_state
    
    # 状态转换逻辑
    if current_state == STATE_ACTIVE:
        if load_current < SLEEP_THRESHOLD:
            new_state = STATE_SLEEP
            mock_set_control_reg(0x01)  # 设置休眠标志
        else:
            mock_set_control_reg(0x00)  # 保持活跃标志
    else:  # STATE_SLEEP
        if load_current > WAKE_THRESHOLD:
            new_state = STATE_ACTIVE
            mock_set_control_reg(0x02)  # 设置唤醒标志
        else:
            mock_set_control_reg(0x01)  # 保持休眠标志
    
    # 如果状态发生变化，记录转换
    if new_state != current_state:
        if current_state == STATE_ACTIVE and new_state == STATE_SLEEP:
            print(f"状态: ACTIVE -> SLEEP, 电流: {load_current} mA")
        elif current_state == STATE_SLEEP and new_state == STATE_ACTIVE:
            print(f"状态: SLEEP -> ACTIVE, 电流: {load_current} mA")
        current_state = new_state
        transition_count += 1

def get_state_name(state):
    """状态名称转换函数"""
    return "ACTIVE" if state == STATE_ACTIVE else "SLEEP"

def main():
    print("DC/DC变换器休眠唤醒控制模拟程序")
    print("================================\n")
    
    # 初始化状态为ACTIVE
    global current_state, transition_count
    current_state = STATE_ACTIVE
    transition_count = 0
    mock_set_control_reg(0x00)  # 初始化为活跃状态
    
    # 测试用例1: 从高负载到低负载再到高负载
    print("测试用例1: 负载变化序列 [30, 10, 40] mA")
    test_case1 = [30, 10, 40]
    for current in test_case1:
        dcdc_state_machine(current)
    
    # 测试用例2: 在阈值附近波动
    print("\n测试用例2: 负载变化序列 [20, 14, 16, 26, 24] mA")
    test_case2 = [20, 14, 16, 26, 24]
    for current in test_case2:
        dcdc_state_machine(current)
    
    # 测试用例3: 长时间低负载后唤醒
    print("\n测试用例3: 负载变化序列 [5, 8, 12, 30, 35] mA")
    test_case3 = [5, 8, 12, 30, 35]
    for current in test_case3:
        dcdc_state_machine(current)
    
    # 打印最终结果
    print("\n================================\n")
    print(f"最终状态: {get_state_name(current_state)}")
    print(f"总转换次数: {transition_count}")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())