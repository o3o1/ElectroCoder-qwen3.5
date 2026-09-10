Write-Host "Grid Inverter Frequency Adaptive Control Test"
Write-Host "=============================================="

$test_cases = @(49.0, 50.2, 52.5, 44.0)

for ($i = 0; $i -lt $test_cases.Count; $i++) {
    $measured_freq = $test_cases[$i]
    Write-Host "`nTest Case $($i+1): Measured Frequency $measured_freq Hz"
    
    # detect_grid_frequency
    $current_freq = $measured_freq
    $is_locked = ($measured_freq -ge 45.0 -and $measured_freq -le 55.0)
    
    # adaptive_control
    $target_freq = 0.0
    if ($is_locked) {
        $freq = $current_freq
        
        if ($freq -ge 49.5 -and $freq -le 50.5) {
            $target_freq = 50.0
        } elseif ($freq -ge 45.0 -and $freq -lt 49.5) {
            $target_freq = 50.0 + (50.0 - $freq) * 0.3
        } elseif ($freq -gt 50.5 -and $freq -le 55.0) {
            $target_freq = 50.0 - ($freq - 50.0) * 0.3
        }
        
        if ($target_freq -lt 45.0) { $target_freq = 45.0 }
        if ($target_freq -gt 55.0) { $target_freq = 55.0 }
    }
    
    # update_phase
    $phase_error = 0.0
    if ($is_locked) {
        $phase_error = ($target_freq - $current_freq) * 10.0
    }
    
    Write-Host "  Current Frequency: $([math]::Round($current_freq, 2)) Hz"
    Write-Host "  Target Frequency: $([math]::Round($target_freq, 2)) Hz"
    Write-Host "  Lock Status: $(if ($is_locked) {'Locked'} else {'Not Locked'})"
    Write-Host "  Phase Error: $([math]::Round($phase_error, 2)) degrees"
}

Write-Host "`nTest Completed!"