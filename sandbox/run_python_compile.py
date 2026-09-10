import subprocess
import os

# 编译C程序
print("Compiling C program...")
compile_result = subprocess.run(["gcc", "task_inverter_fault_fixed.c", "-o", "task_inverter_fault_fixed.exe"], 
                               capture_output=True, text=True)

if compile_result.returncode != 0:
    print("Compilation failed!")
    print("Error:", compile_result.stderr)
else:
    print("Compilation successful!")
    
    # 运行程序
    print("\nRunning program...")
    run_result = subprocess.run(["task_inverter_fault_fixed.exe"], 
                               capture_output=True, text=True)
    
    print("Output:")
    print(run_result.stdout)
    if run_result.stderr:
        print("Errors:", run_result.stderr)