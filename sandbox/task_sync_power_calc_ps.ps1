$PI = [math]::PI
$SAMPLE_RATE = 10000.0
$GRID_FREQ = 50.0
$GRID_VOLTAGE = 220.0
$SAMPLE_PERIOD = 1.0 / $SAMPLE_RATE
$OMEGA_NOMINAL = 2.0 * $PI * $GRID_FREQ

class PLLessSync {
    [float]$theta
    [float]$freq
    [float]$u_alpha_prev
    [float]$u_beta_prev
    [float]$integral
    [float]$kp
    [float]$ki
    
    PLLessSync() {
        $this.theta = 0.0
        $this.freq = $GRID_FREQ
        $this.u_alpha_prev = 0.0
        $this.u_beta_prev = 0.0
        $this.integral = 0.0
        $this.kp = 100.0
        $this.ki = 5000.0
    }
}

function ClarkeTransform($ua, $ub, $uc) {
    $u_alpha = (2.0/3.0) * ($ua - 0.5*$ub - 0.5*$uc)
    $u_beta = (2.0/3.0) * (0.86602540378*$ub - 0.86602540378*$uc)
    return $u_alpha, $u_beta
}

function PLLessSyncUpdate([ref]$sync, $ua, $ub, $uc) {
    $u_alpha, $u_beta = ClarkeTransform $ua $ub $uc
    
    $q = $u_alpha * $sync.Value.u_beta_prev - $u_beta * $sync.Value.u_alpha_prev
    $u_sq = $u_alpha*$u_alpha + $u_beta*$u_beta
    
    $phase_error = 0.0
    if ($u_sq -gt 1e-6) {
        $phase_error = $q / $u_sq
    }
    
    $delta_theta = $sync.Value.kp * $phase_error + $sync.Value.ki * $sync.Value.integral
    $sync.Value.integral += $phase_error * $SAMPLE_PERIOD
    $sync.Value.theta += ($OMEGA_NOMINAL + $delta_theta) * $SAMPLE_PERIOD
    
    while ($sync.Value.theta -gt $PI) { $sync.Value.theta -= 2.0 * $PI }
    while ($sync.Value.theta -lt -$PI) { $sync.Value.theta += 2.0 * $PI }
    
    $sync.Value.freq = $GRID_FREQ + $delta_theta / (2.0 * $PI)
    $sync.Value.u_alpha_prev = $u_alpha
    $sync.Value.u_beta_prev = $u_beta
}

function GenerateThreePhaseVoltage($time) {
    $omega = 2.0 * $PI * $GRID_FREQ
    $phase = $omega * $time
    
    $ua = $GRID_VOLTAGE * [math]::Sin($phase)
    $ub = $GRID_VOLTAGE * [math]::Sin($phase - 2.0*$PI/3.0)
    $uc = $GRID_VOLTAGE * [math]::Sin($phase + 2.0*$PI/3.0)
    
    return $ua, $ub, $uc
}

Write-Host "PLL-less Sync Control Simulation"
Write-Host "Sampling Rate: $SAMPLE_RATE Hz, Grid Freq: $GRID_FREQ Hz, Voltage: $GRID_VOLTAGE V"
Write-Host "============================================================"
Write-Host "Sample`tTime(s)`tPhase(rad)`tFreq(Hz)`tFreqError(Hz)"
Write-Host "============================================================"

$sync = [PLLessSync]::new()
$total_samples = [math]::Floor(5.0 / $GRID_FREQ * $SAMPLE_RATE)
$time = 0.0
$freq_sum = 0.0
$valid_count = 0

for ($i = 0; $i -lt $total_samples; $i++) {
    $ua, $ub, $uc = GenerateThreePhaseVoltage $time
    PLLessSyncUpdate ([ref]$sync) $ua $ub $uc
    $freq_error = $sync.freq - $GRID_FREQ
    
    if ($i -gt 100) {
        $freq_sum += [math]::Abs($freq_error)
        $valid_count++
    }
    
    if ($i % 100 -eq 0) {
        Write-Host ("{0}`t{1:F4}`t{2:F4}`t`t{3:F4}`t`t{4:F4}" -f $i, $time, $sync.theta, $sync.freq, $freq_error)
    }
    
    $time += $SAMPLE_PERIOD
}

$avg_freq_error = if ($valid_count -gt 0) { $freq_sum / $valid_count } else { 0.0 }

Write-Host "============================================================"
Write-Host "Simulation complete! Processed $total_samples samples ($([math]::Round($total_samples * $GRID_FREQ / $SAMPLE_RATE, 1)) cycles)"
Write-Host "Average frequency error: $($avg_freq_error.ToString('F6')) Hz"
Write-Host "Relative error: $([math]::Round($avg_freq_error / $GRID_FREQ * 100.0, 4))%"