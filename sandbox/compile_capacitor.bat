@echo off  
gcc -Wall -std=c99 task_capacitor_balance.c -o task_capacitor_balance.exe  
if 0 neq 0 (  
echo Compilation failed!  
exit /b 1  
)  
task_capacitor_balance.exe 
