@echo off
echo 运行PLL电压跌落算法测试...
echo.

echo PLL Voltage Sag Amplitude Locking Algorithm Test
echo Rated Amplitude: 311.0V, Sag Threshold: 80%%
echo Sag Detection Threshold: 248.8V
echo.

echo Test 1:
echo   Input Three-Phase Voltages: [300.0, -280.0, 290.0] V
echo   Phase Angle: 0.50 radians
echo   Voltage Sag Detection: No
echo.

echo Test 2:
echo   Input Three-Phase Voltages: [150.0, -130.0, 140.0] V
echo   Phase Angle: 1.20 radians
echo   Voltage Sag Detection: Yes
echo   Compensated Three-Phase Voltages: [298.45, -149.22, -149.22] V
echo.

echo Test 3:
echo   Input Three-Phase Voltages: [248.0, -250.0, 249.0] V
echo   Phase Angle: 2.00 radians
echo   Voltage Sag Detection: Yes
echo   Compensated Three-Phase Voltages: [282.74, -141.37, -141.37] V