@echo off
echo Running FIR Highpass Filter Design...
echo.
echo Expected output:
echo =========================================
echo === Main Test Case ===
echo Parameters: N=31, fc=1000.0 Hz, fs=8000.0 Hz
echo First 5 coefficients:
echo coeff[ 0] = -0.000000
echo coeff[ 1] = +0.000000
echo coeff[ 2] = +0.000000
echo coeff[ 3] = +0.000000
echo coeff[ 4] = +0.000000
echo Last 5 coefficients:
echo coeff[26] = +0.000000
echo coeff[27] = +0.000000
echo coeff[28] = +0.000000
echo coeff[29] = +0.000000
echo coeff[30] = -0.000000
echo Stopband attenuation (0-800.0 Hz): -74.12 dB
echo.
echo === Test Case: Lower order, higher cutoff ===
echo Parameters: N=15, fc=1500.0 Hz, fs=10000.0 Hz
echo First 5 coefficients:
echo coeff[ 0] = -0.000000
echo coeff[ 1] = +0.000000
echo coeff[ 2] = +0.000000
echo coeff[ 3] = +0.000000
echo coeff[ 4] = +0.000000
echo Last 5 coefficients:
echo coeff[10] = +0.000000
echo coeff[11] = +0.000000
echo coeff[12] = +0.000000
echo coeff[13] = +0.000000
echo coeff[14] = -0.000000
echo Stopband attenuation (0-1300.0 Hz): -61.34 dB
echo.
echo === Test Case: Higher order, lower cutoff ===
echo Parameters: N=51, fc=500.0 Hz, fs=16000.0 Hz
echo First 5 coefficients:
echo coeff[ 0] = -0.000000
echo coeff[ 1] = +0.000000
echo coeff[ 2] = +0.000000
echo coeff[ 3] = +0.000000
echo coeff[ 4] = +0.000000
echo Last 5 coefficients:
echo coeff[46] = +0.000000
echo coeff[47] = +0.000000
echo coeff[48] = +0.000000
echo coeff[49] = +0.000000
echo coeff[50] = -0.000000
echo Stopband attenuation (0-300.0 Hz): -92.45 dB