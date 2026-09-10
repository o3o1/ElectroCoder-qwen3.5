import subprocess
import os
import sys

def compile_and_run_c_file(c_file_path):
    """编译并运行C文件"""
    try:
        # 检查文件是否存在
        if not os.path.exists(c_file_path):
            print(f"错误: 文件 {c_file_path} 不存在")
            return False
        
        # 尝试使用不同的编译器
        compilers = [
            "gcc", 
            "clang", 
            "cl.exe", 
            "x86_64-w64-mingw32-gcc",
            "i686-w64-mingw32-gcc"
        ]
        
        compiler_found = None
        for compiler in compilers:
            try:
                result = subprocess.run([compiler, "--version"], 
                                      capture_output=True, 
                                      text=True, 
                                      timeout=2)
                if result.returncode == 0:
                    compiler_found = compiler
                    print(f"找到编译器: {compiler}")
                    break
            except (subprocess.TimeoutExpired, FileNotFoundError):
                continue
        
        if not compiler_found:
            print("错误: 未找到C编译器")
            print("请安装以下编译器之一:")
            print("1. MinGW-w64 (推荐)")
            print("2. MSVC (Visual Studio)")
            print("3. Clang")
            return False
        
        # 编译C文件
        exe_file = c_file_path.replace('.c', '.exe')
        compile_cmd = [compiler_found, "-Wall", c_file_path, "-o", exe_file]
        
        print(f"编译命令: {' '.join(compile_cmd)}")
        compile_result = subprocess.run(compile_cmd, 
                                      capture_output=True, 
                                      text=True)
        
        if compile_result.returncode != 0:
            print("编译失败:")
            print(compile_result.stderr)
            return False
        
        print("编译成功!")
        
        # 运行可执行文件
        if os.path.exists(exe_file):
            print(f"\n运行 {exe_file}:")
            print("=" * 50)
            
            run_result = subprocess.run([exe_file], 
                                      capture_output=True, 
                                      text=True)
            
            print(run_result.stdout)
            if run_result.stderr:
                print("错误输出:")
                print(run_result.stderr)
            
            print("=" * 50)
            return True
        else:
            print(f"错误: 可执行文件 {exe_file} 未生成")
            return False
            
    except Exception as e:
        print(f"发生错误: {e}")
        return False

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("用法: python run_c_program.py <c_file_path>")
        sys.exit(1)
    
    c_file = sys.argv[1]
    success = compile_and_run_c_file(c_file)
    
    if not success:
        print("\n尝试直接模拟C程序输出...")
        print("=" * 50)
        
        # 直接模拟程序输出
        test_loads = [0.15, 0.45, 0.85]
        
        def calculate_frequency(load_ratio):
            if load_ratio < 0.3:
                return 30.0
            elif load_ratio <= 0.7:
                return 30.0 + (load_ratio - 0.3) * 25.0
            else:
                return 55.0
        
        for load in test_loads:
            freq = calculate_frequency(load)
            print(f"\nLoad ratio: {load:.2f} -> Target frequency: {freq:.2f} Hz")
            print("Adjusting PWM duty cycle...")
            
            PWM_MAX_VALUE = 3000
            MAX_FREQ = 55.0
            freq_ratio = freq / MAX_FREQ
            target_pwm = int(freq_ratio * PWM_MAX_VALUE)
            
            current_pwm = 1000
            while current_pwm != target_pwm:
                if current_pwm < target_pwm:
                    current_pwm += 100
                    if current_pwm > target_pwm:
                        current_pwm = target_pwm
                else:
                    current_pwm -= 100
                    if current_pwm < target_pwm:
                        current_pwm = target_pwm
                
                print(f"Current value: {current_pwm}")
            
            print(f"Frequency stabilized at {freq:.2f} Hz")
        
        print("=" * 50)
        print("注意: 这是模拟输出，实际C程序需要编译器才能运行")