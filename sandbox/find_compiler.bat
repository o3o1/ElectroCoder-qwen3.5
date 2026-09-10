@echo off  
where gcc  
if not errorlevel 1 echo Found gcc  
where cl  
if not errorlevel 1 echo Found cl  
where cc  
if not errorlevel 1 echo Found cc  
pause 
