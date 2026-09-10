@echo off  
echo Compiling simple insulation test...  
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.38.33130\bin\Hostx64\x64\cl.exe" (  
  "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.38.33130\bin\Hostx64\x64\cl.exe" task_insulation_bridge_simple.c /Fe:insulation_simple.exe  
  if exist insulation_simple.exe insulation_simple.exe  
) else (  
  echo No compiler found  
) 
