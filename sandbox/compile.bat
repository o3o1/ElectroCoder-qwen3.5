@echo off  
echo Trying to compile C program...  
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.38.33130\bin\Hostx64\x64\cl.exe" (  
  call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"  
  cl task_26906_ascii.c /Fetask_26906_ascii.exe  
) else (  
  echo No compiler found  
)  
if exist task_26906_ascii.exe (  
  task_26906_ascii.exe  
) 
