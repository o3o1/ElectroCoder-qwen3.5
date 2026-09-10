@echo off
echo Running Boost PID Controller Simulation
echo ========================================

echo.
echo PID Controller Structure:
echo - Kp: Proportional gain
echo - Ki: Integral gain  
echo - Kd: Derivative gain
echo - integral: Accumulated integral term
echo - prev_error: Previous error for derivative

echo.
echo Simulation Parameters:
echo - Vin: 12.0V
echo - Target Vout: 24.0V
echo - Initial Duty: 0.5
echo - Time step: 0.001s
echo - Noise amplitude: 0.1V

echo.
echo Simulation Results:
echo Time(s)   Current Vout(V)   Error(V)   Duty
echo --------------------------------------------

setlocal enabledelayedexpansion

rem Initialize variables
set /a seed=%random%
set /a i=0
set time=0.0000
set duty=0.5000
set integral=0.0000
set prev_error=0.0000

rem PID parameters
set Kp=0.1
set Ki=0.05
set Kd=0.01

:loop
if %i% geq 5 goto :end

rem Generate noise (simplified)
set /a noise_val=%random% %% 21 - 10
set /a noise_val=!noise_val! * 10
set noise=0.!noise_val!

rem Calculate Vout: Vout = Vin/(1-D) + noise
set /a "vin=120"
set /a "denom=1000 - !duty:~2!"
set /a "vout_base=vin * 1000 / denom"
set vout=!vout_base!.!noise:~2!

rem Calculate error
set /a "target=2400"
set /a "error_val=target - !vout_base!"
set error=!error_val!.00

rem PID calculation
rem Proportional
set /a "prop_val=!error_val! * !Kp:~2!"
set prop=0.!prop_val!

rem Integral
set /a "integral_val=!integral! + !error_val! * 1"
set integral=!integral_val!
set /a "int_val=!integral! * !Ki:~2!"
set integral_term=0.!int_val!

rem Derivative
set /a "deriv_val=(!error_val! - !prev_error!) * !Kd:~2! / 1"
set derivative=0.!deriv_val!
set prev_error=!error_val!

rem PID output
set /a "pid_out_val=!prop_val! + !int_val! + !deriv_val!"
set pid_output=0.!pid_out_val!

rem Update duty
set /a "duty_val=!duty:~2! + !pid_out_val!"
if !duty_val! lss 50 set duty_val=50
if !duty_val! gtr 950 set duty_val=950
set duty=0.!duty_val!

rem Display results
echo !time!     !vout!          !error!     !duty!

rem Update time and counter
set /a "time_val=!time:~0,1! * 1000 + !time:~2!"
set /a "time_val=!time_val! + 1"
set time=0.!time_val!
set /a i+=1
goto :loop

:end
echo.
echo Simulation completed.
pause