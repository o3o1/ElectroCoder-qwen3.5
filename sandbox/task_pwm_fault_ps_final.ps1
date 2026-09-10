class PWM_Monitor {
    [int]$period_ms
    [int]$last_edge_time
    [int]$missing_count
    [int]$fault_flag
    
    PWM_Monitor([int]$period) {
        $this.period_ms = $period
        $this.last_edge_time = 0
        $this.missing_count = 0
        $this.fault_flag = 0
    }
}

function pwm_monitor_init([PWM_Monitor]$mon, [int]$period_ms) {
    $mon.period_ms = $period_ms
    $mon.last_edge_time = 0
    $mon.missing_count = 0
    $mon.fault_flag = 0
}

function pwm_edge_detected([PWM_Monitor]$mon, [int]$current_time) {
    $mon.last_edge_time = $current_time
    $mon.missing_count = 0
}

function check_pwm_missing([PWM_Monitor]$mon, [int]$current_time) {
    if ($mon.fault_flag -eq 1) {
        return 1
    }
    
    if ($mon.last_edge_time -eq 0) {
        return 0
    }
    
    $time_since_last_edge = $current_time - $mon.last_edge_time
    
    if ($time_since_last_edge -gt ($mon.period_ms * 1.5)) {
        $mon.missing_count++
        
        if ($mon.missing_count -ge 3) {
            $mon.fault_flag = 1
        }
        
        return 1
    }
    
    return 0
}

function main {
    $monitor = [PWM_Monitor]::new(20)
    pwm_monitor_init $monitor 20
    
    $edge_times = @(5, 25, 45, 65, 85)
    $edge_count = 5
    $edge_index = 0
    
    for ($time_ms = 0; $time_ms -le 150; $time_ms++) {
        if ($edge_index -lt $edge_count -and $time_ms -eq $edge_times[$edge_index]) {
            pwm_edge_detected $monitor $time_ms
            $edge_index++
        }
        
        $missing = check_pwm_missing $monitor $time_ms
        
        if ($missing -eq 1) {
            Write-Host "Time ${time_ms}ms: Missing detected! Count=$($monitor.missing_count)" -NoNewline
            if ($monitor.fault_flag -eq 1) {
                Write-Host " FAULT!" -NoNewline
            }
            Write-Host ""
        }
    }
}

main