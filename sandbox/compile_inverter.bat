@echo off  
gcc -Wall -lm task_inverter_distortion.c -o task_inverter_distortion.exe  
if errorlevel 1 echo Compilation failed  
task_inverter_distortion.exe 
