import subprocess
import os
import sys

# 首先尝试用gcc编译
try:
    # 编译C程序
    result = subprocess.run(['gcc', 'task_watchdog_fixed.c', '-o', 'task_watchdog_fixed.exe'], 
                          capture_output=True, text=True)
    if result.returncode == 0:
        # 运行程序
        run_result = subprocess.run(['task_watchdog_fixed.exe'], 
                                  capture_output=True, text=True)
        print(run_result.stdout)
        if run_result.stderr:
            print("错误:", run_result.stderr)
    else:
        print("编译失败:", result.stderr)
except FileNotFoundError:
    print("gcc未找到，尝试其他方法...")
    # 尝试用cl编译
    try:
        result = subprocess.run(['cl', 'task_watchdog_fixed.c'], 
                              capture_output=True, text=True)
        if result.returncode == 0:
            # 运行程序
            run_result = subprocess.run(['task_watchdog_fixed.exe'], 
                                      capture_output=True, text=True)
            print(run_result.stdout)
            if run_result.stderr:
                print("错误:", run_result.stderr)
        else:
            print("cl编译失败:", result.stderr)
    except FileNotFoundError:
        print("cl未找到，尝试直接运行Python模拟...")
        # 直接模拟C程序逻辑
        import random
        import time
        
        random.seed(time.time())
        
        class Watchdog:
            def __init__(self, threshold):
                self.feed_counter = 0
                self.timeout_threshold = threshold
                self.power_status = 1
        
        def watchdog_init(wd, threshold):
            wd.feed_counter = 0
            wd.timeout_threshold = threshold
            wd.power_status = 1
        
        def watchdog_feed(wd):
            wd.feed_counter = 0
        
        def check_power_failure(cycle):
            if cycle == 3:
                return 0
            return 1 if random.randint(0, 9) > 1 else 0
        
        def system_reboot(wd):
            print("触发系统重启...")
            watchdog_init(wd, wd.timeout_threshold)
        
        wd = Watchdog(3)
        
        for cycle in range(1, 6):
            print(f"周期{cycle}: ", end="")
            
            wd.power_status = check_power_failure(cycle)
            
            if wd.power_status == 1:
                watchdog_feed(wd)
                print(f"电源正常 | 喂狗成功 | 计数器:{wd.feed_counter}")
            else:
                print("电源故障！", end="")
                system_reboot(wd)
            
            wd.feed_counter += 1
            
            if wd.feed_counter >= wd.timeout_threshold:
                print("看门狗超时！", end="")
                system_reboot(wd)