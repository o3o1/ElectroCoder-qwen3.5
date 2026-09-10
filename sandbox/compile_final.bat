@echo off  
echo Compiling cascade PID controller...  
cl task_26906_simple.c /Fetask_26906_simple.exe  
if exist task_26906_simple.exe (  
  echo Running program...  
  task_26906_simple.exe  
) else (  
  echo Compilation failed  
) 
