@echo off
chcp 65001 >nul
echo 正在运行看门狗模拟程序...
echo.

REM 直接模拟C程序逻辑
python -c "
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
    print('触发系统重启...')
    watchdog_init(wd, wd.timeout_threshold)

wd = Watchdog(3)

for cycle in range(1, 6):
    print(f'周期{cycle}: ', end='')
    
    wd.power_status = check_power_failure(cycle)
    
    if wd.power_status == 1:
        watchdog_feed(wd)
        print(f'电源正常 | 喂狗成功 | 计数器:{wd.feed_counter}')
    else:
        print('电源故障！', end='')
        system_reboot(wd)
    
    wd.feed_counter += 1
    
    if wd.feed_counter >= wd.timeout_threshold:
        print('看门狗超时！', end='')
        system_reboot(wd)
"

pause