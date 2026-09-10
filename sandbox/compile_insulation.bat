@echo off  
where gcc  
if %0% equ 0 (  
  echo Using gcc...  
  gcc -Wall -lm task_insulation_bridge_fixed.c -o insulation_test.exe  
  if %0% equ 0 (  
    echo Compile successful  
    insulation_test.exe  
  ) else (  
    echo gcc compile failed  
  )  
  goto :end  
)  
where cl  
if %0% equ 0 (  
  echo Using cl...  
  cl task_insulation_bridge_fixed.c /Fe:insulation_test.exe  
  if %0% equ 0 (  
    echo Compile successful  
    insulation_test.exe  
  ) else (  
    echo cl compile failed  
  )  
  goto :end  
)  
where cc  
if %0% equ 0 (  
  echo Using cc...  
  cc task_insulation_bridge_fixed.c -o insulation_test.exe  
  if %0% equ 0 (  
    echo Compile successful  
    insulation_test.exe  
  ) else (  
    echo cc compile failed  
  )  
  goto :end  
)  
echo No C compiler found  
:end  
pause 
