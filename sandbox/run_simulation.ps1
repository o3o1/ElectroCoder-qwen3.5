Write-Host "Running PID Feedback Linearization Simulation..."
Write-Host "Time(s)`t`tx`t`tu`t`terror"
Write-Host "--------------------------------------------"

$TS = 0.001
$STEPS = 1000
$PRINT_INTERVAL = 100
$Kp = 5.0
$Ki = 0.5
$Kd = 0.1
$x = 0.5
$x_ref = 1.0
$x_ref_derivative = 0.0
$integral = 0.0
$prev_error = 0.0

function f($x) {
    return -$x*$x + 1.0
}

function g($x) {
    return 2.0 + [Math]::Sin($x)
}

for ($i = 0; $i -le $STEPS; $i++) {
    $t = $i * $TS
    $error = $x_ref - $x
    $integral += $error * $TS
    $derivative = ($error - $prev_error) / $TS
    
    $u = (1.0 / (g $x)) * (-(f $x) - $Kp * $error - $Ki * $integral - $Kd * $derivative + $x_ref_derivative)
    
    $dx = (f $x) + (g $x) * $u
    $x += $dx * $TS
    
    if ($i % $PRINT_INTERVAL -eq 0) {
        Write-Host ("{0:F6}`t{1:F6}`t{2:F6}`t{3:F6}" -f $t, $x, $u, $error)
    }
    
    $prev_error = $error
}