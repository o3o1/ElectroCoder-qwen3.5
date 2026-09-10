@echo off
echo Running C program directly...
echo.

REM 创建临时C文件
echo #include <stdio.h> > temp.c
echo. >> temp.c
echo typedef struct { >> temp.c
echo     float dc_voltage; >> temp.c
echo     float output_current; >> temp.c
echo     float temperature; >> temp.c
echo     int fault_flag; >> temp.c
echo } InverterStatus; >> temp.c
echo. >> temp.c
echo int check_fault_protection(InverterStatus *inv) { >> temp.c
echo     if (inv->dc_voltage > 800.0f) { >> temp.c
echo         inv->fault_flag = 1; >> temp.c
echo         return 1; >> temp.c
echo     } >> temp.c
echo     if (inv->dc_voltage < 450.0f) { >> temp.c
echo         inv->fault_flag = 1; >> temp.c
echo         return 2; >> temp.c
echo     } >> temp.c
echo     if (inv->output_current > 20.0f) { >> temp.c
echo         inv->fault_flag = 1; >> temp.c
echo         return 3; >> temp.c
echo     } >> temp.c
echo     if (inv->temperature > 85.0f) { >> temp.c
echo         inv->fault_flag = 1; >> temp.c
echo         return 4; >> temp.c
echo     } >> temp.c
echo     inv->fault_flag = 0; >> temp.c
echo     return 0; >> temp.c
echo } >> temp.c
echo. >> temp.c
echo int main() { >> temp.c
echo     InverterStatus test1 = {500.0f, 15.0f, 75.0f, 0}; >> temp.c
echo     InverterStatus test2 = {900.0f, 18.0f, 70.0f, 0}; >> temp.c
echo     InverterStatus test3 = {600.0f, 25.0f, 90.0f, 0}; >> temp.c
echo. >> temp.c
echo     int result1 = check_fault_protection(&test1); >> temp.c
echo     int result2 = check_fault_protection(&test2); >> temp.c
echo     int result3 = check_fault_protection(&test3); >> temp.c
echo. >> temp.c
echo     printf("Test 1: Voltage=%.0fV, Current=%.0fA, Temperature=%.0fC -> Status:", >> temp.c
echo            test1.dc_voltage, test1.output_current, test1.temperature); >> temp.c
echo     if (result1 == 0) { >> temp.c
echo         printf("Normal\n"); >> temp.c
echo     } else if (result1 == 1) { >> temp.c
echo         printf("Fault(Overvoltage)\n"); >> temp.c
echo     } else if (result1 == 2) { >> temp.c
echo         printf("Fault(Undervoltage)\n"); >> temp.c
echo     } else if (result1 == 3) { >> temp.c
echo         printf("Fault(Overcurrent)\n"); >> temp.c
echo     } else if (result1 == 4) { >> temp.c
echo         printf("Fault(Overtemperature)\n"); >> temp.c
echo     } >> temp.c
echo. >> temp.c
echo     printf("Test 2: Voltage=%.0fV, Current=%.0fA, Temperature=%.0fC -> Status:", >> temp.c
echo            test2.dc_voltage, test2.output_current, test2.temperature); >> temp.c
echo     if (result2 == 0) { >> temp.c
echo         printf("Normal\n"); >> temp.c
echo     } else if (result2 == 1) { >> temp.c
echo         printf("Fault(Overvoltage)\n"); >> temp.c
echo     } else if (result2 == 2) { >> temp.c
echo         printf("Fault(Undervoltage)\n"); >> temp.c
echo     } else if (result2 == 3) { >> temp.c
echo         printf("Fault(Overcurrent)\n"); >> temp.c
echo     } else if (result2 == 4) { >> temp.c
echo         printf("Fault(Overtemperature)\n"); >> temp.c
echo     } >> temp.c
echo. >> temp.c
echo     printf("Test 3: Voltage=%.0fV, Current=%.0fA, Temperature=%.0fC -> Status:", >> temp.c
echo            test3.dc_voltage, test3.output_current, test3.temperature); >> temp.c
echo     if (result3 == 0) { >> temp.c
echo         printf("Normal\n"); >> temp.c
echo     } else if (result3 == 1) { >> temp.c
echo         printf("Fault(Overvoltage)\n"); >> temp.c
echo     } else if (result3 == 2) { >> temp.c
echo         printf("Fault(Undervoltage)\n"); >> temp.c
echo     } else if (result3 == 3) { >> temp.c
echo         printf("Fault(Overcurrent)\n"); >> temp.c
echo     } else if (result3 == 4) { >> temp.c
echo         printf("Fault(Overtemperature)\n"); >> temp.c
echo     } >> temp.c
echo. >> temp.c
echo     return 0; >> temp.c
echo } >> temp.c

REM 尝试编译
echo Attempting to compile...
gcc temp.c -o temp.exe 2>nul

if exist temp.exe (
    echo Compilation successful!
    echo.
    temp.exe
    del temp.exe
) else (
    echo No C compiler found. Showing code instead:
    echo.
    type temp.c
)

del temp.c 2>nul