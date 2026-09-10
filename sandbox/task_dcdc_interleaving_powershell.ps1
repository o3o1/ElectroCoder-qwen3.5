Add-Type -TypeDefinition @"
using System;
using System.Runtime.InteropServices;

public class DllImporter {
    [DllImport("msvcrt.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern int printf(string format);
    
    [DllImport("msvcrt.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern double sin(double x);
    
    [DllImport("msvcrt.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern double fabs(double x);
}
"@

function CalculateRippleCurrent {
    param($I_pp, $N, $D)
    
    if ($D -eq 0.0 -or $D -eq 1.0) {
        return 0.0
    }
    
    if ($N -le 0) {
        return $I_pp
    }
    
    $k = [DllImporter]::fabs([DllImporter]::sin($N * [Math]::PI * $D)) / ($N * [DllImporter]::sin([Math]::PI * $D))
    return $I_pp * $k
}

function PhaseDelayCalculation {
    param($N, $T_sw, $i)
    
    if ($N -le 0 -or $i -lt 0 -or $i -ge $N) {
        return 0.0
    }
    return $i * $T_sw / $N
}

$test_cases = @(
    @{N=3; D=0.4; I_pp=2.0; T_sw=1.0e-5},
    @{N=4; D=0.6; I_pp=1.5; T_sw=2.0e-5},
    @{N=2; D=0.3; I_pp=3.0; T_sw=5.0e-6}
)

foreach ($test in $test_cases) {
    $total_ripple = CalculateRippleCurrent $test.I_pp $test.N $test.D
    $output = "N=$($test.N), D=$($test.D.ToString('F2')), I_pp_single=$($test.I_pp.ToString('F1'))A => total ripple: $($total_ripple.ToString('F3'))A, phase delay: ["
    
    $delays = @()
    for ($i = 0; $i -lt $test.N; $i++) {
        $delay = PhaseDelayCalculation $test.N $test.T_sw $i
        $delays += "$($delay.ToString('F6'))s"
    }
    
    $output += ($delays -join ", ") + "]"
    Write-Host $output
}