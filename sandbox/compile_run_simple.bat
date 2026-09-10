@echo off
echo Running PID Feedback Linearization Simulation...
echo Time(s) x u error
echo --------------------------------------------

set TS=0.001
set STEPS=1000
set PRINT_INTERVAL=100
set Kp=5.0
set Ki=0.5
set Kd=0.1
set x=0.5
set x_ref=1.0
set x_ref_derivative=0.0
set integral=0.0
set prev_error=0.0

for /l %%i in (0,1,1000) do (
  set /a i=%%i
  set /a t_int=!i!*1
  set t=!t_int!
  set /a t=!t!/1000
  
  set error=!x_ref!-!x!
  set integral=!integral!+!error!*0.001
  
  set derivative=(!error!-!prev_error!)/0.001
  
  set f=-!x!*!x!+1.0
  set g=2.0+!x!
  
  set u=(1.0/!g!)*(-!f!-5.0*!error!-0.5*!integral!-0.1*!derivative!+0.0)
  
  set dx=!f!+!g!*!u!
  set x=!x!+!dx!*0.001
  
  if !i! %% 100 == 0 (
    echo !t! !x! !u! !error!
  )
  
  set prev_error=!error!
)