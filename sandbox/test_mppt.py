import ctypes
import os

# 编译C代码
os.system('gcc -Wall -lm ./sandbox/task_mppt_po.c -o ./sandbox/task_mppt_po.exe')

# 运行编译后的程序
os.system('./sandbox/task_mppt_po.exe')