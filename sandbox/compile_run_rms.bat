@echo off
echo Compiling RMS calculation program...

REM 尝试使用可用的编译器
if exist "C:\Program Files\mingw64\bin\gcc.exe" (
    echo Using MinGW GCC...
    "C:\Program Files\mingw64\bin\gcc.exe" -Wall -lm task_39455_fixed.c -o task_39455_fixed.exe
) else if exist "C:\MinGW\bin\gcc.exe" (
    echo Using MinGW GCC...
    "C:\MinGW\bin\gcc.exe" -Wall -lm task_39455_fixed.c -o task_39455_fixed.exe
) else (
    echo No C compiler found. Using Python simulation...
    python -c "
import math

def calculate_rms(samples):
    sum_squares = 0.0
    for sample in samples:
        sum_squares += sample * sample
    return math.sqrt(sum_squares / len(samples))

PI = 3.14159265358979323846
samples_per_cycle = 32
cycles = 2
total_samples = samples_per_cycle * cycles
peak_current = 10.0
frequency = 50.0
sampling_freq = 1600.0

current_samples = []
for i in range(total_samples):
    time = i / sampling_freq
    current_samples.append(peak_current * math.sin(2 * PI * frequency * time))

rms_value = calculate_rms(current_samples)

print('AC Current RMS Calculation Result:')
print(f'Number of samples: {total_samples}')
print(f'Calculated RMS value: {rms_value:.6f} A')
print(f'Theoretical RMS value: {peak_current / math.sqrt(2.0):.6f} A')
print(f'Error: {abs(rms_value - peak_current / math.sqrt(2.0)):.6f} A')
"
    goto end
)

if exist "task_39455_fixed.exe" (
    echo Running program...
    task_39455_fixed.exe
) else (
    echo Compilation failed!
)

:end
pause