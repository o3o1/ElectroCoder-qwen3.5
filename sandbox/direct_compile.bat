@echo off  
echo Trying to compile...  
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.38.33130\bin\Hostx64\x64\cl.exe" (  
  echo Found Visual Studio compiler  
  "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.38.33130\bin\Hostx64\x64\cl.exe" task_insulation_bridge_fixed.c /Fe:insulation_test.exe  
  if exist insulation_test.exe (  
    echo Compile successful  
    insulation_test.exe  
  ) else (  
    echo Compile failed  
  )  
) else (  
  echo No compiler found  
)  
pause 
