# PowerShell script to simulate AFD algorithm
$GRID_FREQ_NOMINAL = 50.0
$FREQ_UPPER_LIMIT = 50.5
$FREQ_LOWER_LIMIT = 49.5
$AFD_PERTURBATION = 0.02
$MAX_CYCLES = 10

function Detect-Islanding {
    param([float]$measured_freq)
    
    if ($measured_freq -gt $GRID_FREQ_NOMINAL) {
        return $measured_freq - $AFD_PERTURBATION
    } else {
        return $measured_freq + $AFD_PERTURBATION
    }
}

Write-Host "=== Test 1: Grid Normal Condition ==="
$current_freq = 50.0

for ($i = 0; $i -lt $MAX_CYCLES; $i++) {
    $perturbed = Detect-Islanding -measured_freq $current_freq
    Write-Host "Cycle $($i+1): Measured freq=$($current_freq.ToString('F3'))Hz, Perturbed freq=$($perturbed.ToString('F3'))Hz"
    $current_freq = 50.0
}
Write-Host "Grid normal: Frequency stable around nominal value`n"

Write-Host "=== Test 2: Islanding Scenario ==="
$current_freq = 50.0
$island_detected = $false

for ($cycle = 0; $cycle -lt $MAX_CYCLES; $cycle++) {
    $perturbed = Detect-Islanding -measured_freq $current_freq
    
    Write-Host -NoNewline "Cycle $($cycle+1): Measured freq=$($current_freq.ToString('F3'))Hz, Perturbed freq=$($perturbed.ToString('F3'))Hz"
    
    if ($perturbed -gt $FREQ_UPPER_LIMIT -or $perturbed -lt $FREQ_LOWER_LIMIT) {
        Write-Host " -> Exceeds threshold!"
        $island_detected = $true
        break
    } else {
        Write-Host " -> Within normal range"
    }
    
    $current_freq += 0.1
}

if ($island_detected) {
    Write-Host "Result: Islanding detection triggered"
} else {
    Write-Host "Result: Not triggered (reached max detection cycles)"
}