import subprocess
import os

# 编译C代码
print("正在编译并网逆变器频率自适应控制程序...")

# 创建编译脚本
compile_script = """
@echo off
echo 正在编译并网逆变器频率自适应控制程序...
gcc -Wall -std=c99 task_grid_inverter.c -o task_grid_inverter.exe
if %errorlevel% equ 0 (
    echo 编译成功！
    echo.
    echo 运行结果：
    echo ==============================
    task_grid_inverter.exe
) else (
    echo 编译失败！
)
pause
"""

# 保存编译脚本
with open('./sandbox/compile_grid_inverter.bat', 'w', encoding='gbk') as f:
    f.write(compile_script)

# 尝试运行编译脚本
try:
    result = subprocess.run(['cmd', '/c', 'cd ./sandbox && compile_grid_inverter.bat'], 
                          capture_output=True, text=True, encoding='gbk')
    print(result.stdout)
    if result.stderr:
        print("错误信息:", result.stderr)
except Exception as e:
    print(f"运行编译脚本时出错: {e}")
    
    # 如果编译失败，直接运行Python模拟
    print("\n使用Python模拟C程序逻辑...")
    print("=" * 50)
    
    class GridInverter:
        def __init__(self):
            self.current_freq = 0.0
            self.target_freq = 0.0
            self.is_locked = False
            self.phase_error = 0.0
    
    def detect_grid_frequency(inv, measured_freq):
        inv.current_freq = measured_freq
        inv.is_locked = (45.0 <= measured_freq <= 55.0)
    
    def adaptive_control(inv):
        if not inv.is_locked:
            inv.target_freq = 0.0
            return
        
        freq = inv.current_freq
        
        if 49.5 <= freq <= 50.5:
            inv.target_freq = 50.0
        elif 45.0 <= freq < 49.5:
            inv.target_freq = 50.0 + (50.0 - freq) * 0.3
        elif 50.5 < freq <= 55.0:
            inv.target_freq = 50.0 - (freq - 50.0) * 0.3
        
        if inv.target_freq < 45.0:
            inv.target_freq = 45.0
        if inv.target_freq > 55.0:
            inv.target_freq = 55.0
    
    def update_phase(inv):
        if not inv.is_locked:
            inv.phase_error = 0.0
            return
        inv.phase_error = (inv.target_freq - inv.current_freq) * 10.0
    
    # 测试用例
    inverter = GridInverter()
    test_cases = [49.0, 50.2, 52.5, 44.0]
    
    print("并网逆变器频率自适应控制测试")
    print("=" * 50)
    
    for i, freq in enumerate(test_cases):
        print(f"\n测试用例 {i+1}: 测量频率 {freq:.1f}Hz")
        
        detect_grid_frequency(inverter, freq)
        adaptive_control(inverter)
        update_phase(inverter)
        
        print(f"  当前频率: {inverter.current_freq:.2f} Hz")
        print(f"  目标频率: {inverter.target_freq:.2f} Hz")
        print(f"  锁定状态: {'锁定' if inverter.is_locked else '未锁定'}")
        print(f"  相位误差: {inverter.phase_error:.2f} 度")