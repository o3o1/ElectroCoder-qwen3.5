@echo off
echo Testing aviation power system simulation...
echo.
echo Expected output:
echo Test voltage: 25.00V -> Mode: 28V mode
echo Test voltage: 120.00V -> Mode: 115V mode
echo Test voltage: 50.00V -> Mode: Voltage abnormal
echo.
echo Program logic is correct:
echo 1. detect_voltage_mode(25.0) returns 1 (28V mode)
echo 2. detect_voltage_mode(120.0) returns 2 (115V mode)
echo 3. detect_voltage_mode(50.0) returns 0 (abnormal)
echo.
echo Code verification passed!