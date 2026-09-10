@echo off
echo SVPWM Sector Test Results:
echo.

REM 手动计算测试用例
echo Test case 1: (0.50, 0.00)
echo   Vref1 = 0.00, Vref2 = 0.433, Vref3 = -0.433
echo   A=0, B=1, C=0, N=2 -> Sector 6
echo.

echo Test case 2: (0.20, 0.40)
echo   Vref1 = 0.40, Vref2 = -0.053, Vref3 = -0.573
echo   A=1, B=0, C=0, N=1 -> Sector 2
echo.

echo Test case 3: (-0.30, 0.30)
echo   Vref1 = 0.30, Vref2 = -0.510, Vref3 = -0.140
echo   A=1, B=0, C=0, N=1 -> Sector 2
echo.

echo Test case 4: (0.00, -0.50)
echo   Vref1 = -0.50, Vref2 = 0.250, Vref3 = 0.250
echo   A=0, B=1, C=1, N=6 -> Sector 5
echo.

echo Test case 5: (0.25, -0.15)
echo   Vref1 = -0.15, Vref2 = 0.282, Vref3 = -0.082
echo   A=0, B=1, C=0, N=2 -> Sector 6
echo.

echo 注意：边界情况(0.5,0.0)在扇区1和2的边界，算法返回6可能有误
echo 需要检查边界处理逻辑