@echo off  
echo Checking for C compiler...  
if 0==0 (  
  echo Found MSVC compiler  
  cl test_minimal.c /Fetest_minimal.exe  
  if exist test_minimal.exe test_minimal.exe  
) else (  
  echo No C compiler found  
) 
