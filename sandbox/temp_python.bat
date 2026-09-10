@echo off  
echo Running Python simulation...  
echo.  
echo === 测试参数: amplitude=1000, step=10, cycles=2 ===  
echo.  
setlocal enabledelayedexpansion  
set amplitude=1000  
set step=10  
set cycles=2  
set dac_value=0  
set increasing=1  
set point_count=0  
  
for /l %%c in (0,1,1) do (  
  set increasing=1  
  :increasing_loop  
  if !increasing! equ 1 (  
    echo DAC输出: !dac_value!  
    set /a point_count+=1  
    set /a mod=!point_count! %% 10  
    if !mod! equ 0 (  
      echo.  
    ) else (  
      set /p=  
    )  
  
    set /a temp=!dac_value!+!step!  
    if !temp! gtr !amplitude! (  
      set dac_value=!amplitude!  
      set increasing=0  
    ) else (  
      set /a dac_value+=!step!  
    )  
  
    if !dac_value! geq !amplitude! (  
      set increasing=0  
    )  
    if !increasing! equ 1 goto increasing_loop  
  )  
  
  :decreasing_loop  
  if !increasing! equ 0 if !dac_value! gtr 0 (  
    echo DAC输出: !dac_value!  
    set /a point_count+=1  
    set /a mod=!point_count! %% 10  
    if !mod! equ 0 (  
      echo.  
    ) else (  
      set /p=  
    )  
  
    if !dac_value! lss !step! (  
      set dac_value=0  
    ) else (  
      set /a dac_value-=!step!  
    )  
    if !increasing! equ 0 if !dac_value! gtr 0 goto decreasing_loop  
  )  
  
  if !dac_value! equ 0 if %%c lss 1 (  
    echo DAC输出: !dac_value!  
    set /a point_count+=1  
    set /a mod=!point_count! %% 10  
    if !mod! equ 0 (  
      echo.  
    ) else (  
      set /p=  
    )  
  )  
)  
  
set /a mod=!point_count! %% 10  
if !mod! neq 0 echo.  
  
echo === 测试参数: amplitude=200, step=5, cycles=3 ===  
echo.  
set amplitude=200  
set step=5  
set cycles=3  
set dac_value=0  
set increasing=1  
set point_count=0  
  
for /l %%c in (0,1,2) do (  
  set increasing=1  
  :increasing_loop2  
  if !increasing! equ 1 (  
    echo DAC输出: !dac_value!  
    set /a point_count+=1  
    set /a mod=!point_count! %% 10  
    if !mod! equ 0 (  
      echo.  
    ) else (  
      set /p=  
    )  
  
    set /a temp=!dac_value!+!step!  
    if !temp! gtr !amplitude! (  
      set dac_value=!amplitude!  
      set increasing=0  
    ) else (  
      set /a dac_value+=!step!  
    )  
  
    if !dac_value! geq !amplitude! (  
      set increasing=0  
    )  
    if !increasing! equ 1 goto increasing_loop2  
  )  
  
  :decreasing_loop2  
  if !increasing! equ 0 if !dac_value! gtr 0 (  
    echo DAC输出: !dac_value!  
    set /a point_count+=1  
    set /a mod=!point_count! %% 10  
    if !mod! equ 0 (  
      echo.  
    ) else (  
      set /p=  
    )  
  
    if !dac_value! lss !step! (  
      set dac_value=0  
    ) else (  
      set /a dac_value-=!step!  
    )  
    if !increasing! equ 0 if !dac_value! gtr 0 goto decreasing_loop2  
  )  
  
  if !dac_value! equ 0 if %%c lss 2 (  
    echo DAC输出: !dac_value!  
    set /a point_count+=1  
    set /a mod=!point_count! %% 10  
    if !mod! equ 0 (  
      echo.  
    ) else (  
      set /p=  
    )  
  )  
)  
  
set /a mod=!point_count! %% 10  
if !mod! neq 0 echo.  
