Write-Host "=== Chebyshev Filter Simulation ==="

$PI = [Math]::PI
$fc = 100.0
$fs = 1000.0
$Rp = 1.0

function Calculate-ChebyshevCoeffs {
    param($fc, $fs, $Rp)
    
    $wp = 2.0 * $PI * $fc
    $T = 1.0 / $fs
    
    $epsilon = [Math]::Sqrt([Math]::Pow(10.0, $Rp/10.0) - 1.0)
    $beta = [Math]::Asinh(1.0/$epsilon) / 2.0
    $eta = [Math]::Sinh($beta)
    $xi = [Math]::Cosh($beta)
    
    $wc = 2.0 / $T * [Math]::Tan($wp * $T / 2.0)
    
    $s1_real = -$eta * $wc
    $s1_imag = $xi * $wc
    $s2_real = -$eta * $wc
    $s2_imag = -$xi * $wc
    
    $z1_real = (1.0 + $s1_real * $T/2.0) / (1.0 - $s1_real * $T/2.0)
    $z1_imag = ($s1_imag * $T/2.0) / (1.0 - $s1_real * $T/2.0)
    $z2_real = (1.0 + $s2_real * $T/2.0) / (1.0 - $s2_real * $T/2.0)
    $z2_imag = ($s2_imag * $T/2.0) / (1.0 - $s2_real * $T/2.0)
    
    $z1_mag_sq = $z1_real*$z1_real + $z1_imag*$z1_imag
    $z2_mag_sq = $z2_real*$z2_real + $z2_imag*$z2_imag
    
    $b0 = 1.0
    $b1 = -2.0 * $z1_real
    $b2 = $z1_mag_sq
    
    $a0 = 1.0
    $a1 = -2.0 * $z2_real
    $a2 = $z2_mag_sq
    
    $norm = $a0
    $b0 /= $norm
    $b1 /= $norm
    $b2 /= $norm
    $a1 /= $norm
    $a2 /= $norm
    
    return @($b0, $b1, $b2), @(1.0, $a1, $a2)
}

function Chebyshev-Filter {
    param($input_val, $state, $coeff)
    
    $b0, $b1, $b2, $a1, $a2 = $coeff
    
    $w0 = $input_val - $a1 * $state[0] - $a2 * $state[1]
    $output = $b0 * $w0 + $b1 * $state[0] + $b2 * $state[1]
    
    $state[1] = $state[0]
    $state[0] = $w0
    
    return $output
}

$b, $a = Calculate-ChebyshevCoeffs $fc $fs $Rp

Write-Host "Chebyshev Type I Lowpass Filter Coefficients:"
Write-Host ("b0 = {0:F6}, b1 = {1:F6}, b2 = {2:F6}" -f $b[0], $b[1], $b[2])
Write-Host ("a0 = 1.000000, a1 = {0:F6}, a2 = {1:F6}" -f $a[1], $a[2])

$coeff = @($b[0], $b[1], $b[2], $a[1], $a[2])
$state = @(0.0, 0.0)

Write-Host "`nTest Signal (50Hz + 150Hz sine waves):"
Write-Host "Input samples and filtered output:"

for ($i = 0; $i -lt 10; $i++) {
    $t = $i / $fs
    $input_val = [Math]::Sin(2.0 * $PI * 50.0 * $t) + [Math]::Sin(2.0 * $PI * 150.0 * $t)
    $output = Chebyshev-Filter $input_val $state $coeff
    
    Write-Host ("Sample {0}: Input = {1:F6}, Output = {2:F6}" -f $i, $input_val, $output)
}