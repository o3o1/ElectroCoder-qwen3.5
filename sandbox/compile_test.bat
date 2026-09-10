@echo off  
echo Testing compilation...  
where gcc 2>nul  
if errorlevel 1 echo No gcc found  
where cl 2>nul  
if errorlevel 1 echo No cl found  
where cc 2>nul  
if errorlevel 1 echo No cc found 
