@echo off  
echo 正在编译DAC三角波程序...  
if exist task_dac_triangle_fixed.exe del task_dac_triangle_fixed.exe  
  
echo 尝试使用gcc编译...  
where gcc  
if %0% equ 0 (  
    gcc -Wall -std=c99 task_dac_triangle_fixed.c -o task_dac_triangle_fixed.exe  
    if %0% equ 0 (  
        echo 编译成功！  
        echo.  
        task_dac_triangle_fixed.exe  
    ) else (  
        echo gcc编译失败  
    )  
    goto :end  
)  
  
echo 尝试使用cl编译...  
where cl  
if %0% equ 0 (  
    cl task_dac_triangle_fixed.c /Fe:task_dac_triangle_fixed.exe  
    if %0% equ 0 (  
        echo 编译成功！  
        echo.  
        task_dac_triangle_fixed.exe  
    ) else (  
        echo cl编译失败  
    )  
    goto :end  
)  
  
echo 未找到C编译器  
echo 请安装gcc、cl或cc编译器  
  
:end  
pause 
