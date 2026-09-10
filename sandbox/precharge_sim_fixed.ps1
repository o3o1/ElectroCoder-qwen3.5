Write-Host "Inverter Precharge Control Simulation"
Write-Host "====================================="

# Constants
$DC_BUS_VOLTAGE_MAX = 800
$PRECHARGE_VOLTAGE_THRESHOLD = 600
$PRECHARGE_TIMEOUT_MS = 10000

# State definitions
$STATE_IDLE = 0
$STATE_PRECHARGE = 1
$STATE_RUN = 2
$STATE_FAULT = 3

# Global variables
$mock_dc_voltage = 0
$mock_fault_signal = $false

class PrechargeController {
    [int]$current_state = $STATE_IDLE
    [int]$precharge_start_time = 0
    [bool]$start_command = $false
    [bool]$reset_command = $false
    [bool]$voltage_reached = $false
    [int]$voltage_stable_time = 0
}

function precharge_init([ref]$ctrl) {
    $ctrl.Value.current_state = $STATE_IDLE
    $ctrl.Value.precharge_start_time = 0
    $ctrl.Value.start_command = $false
    $ctrl.Value.reset_command = $false
    $ctrl.Value.voltage_reached = $false
    $ctrl.Value.voltage_stable_time = 0
}

function precharge_run([ref]$ctrl, [int]$current_time) {
    # Check for fault signal in any state
    if ($mock_fault_signal) {
        $ctrl.Value.current_state = $STATE_FAULT
        Write-Host "Fault detected! Entering FAULT state"
    }
    
    switch ($ctrl.Value.current_state) {
        $STATE_IDLE {
            # Check for start command
            if ($ctrl.Value.start_command) {
                $ctrl.Value.current_state = $STATE_PRECHARGE
                $ctrl.Value.precharge_start_time = $current_time
                $ctrl.Value.start_command = $false
                Write-Host "State transition: IDLE -> PRECHARGE"
            }
        }
        
        $STATE_PRECHARGE {
            # Check for timeout
            if (($current_time - $ctrl.Value.precharge_start_time) -ge $PRECHARGE_TIMEOUT_MS) {
                $ctrl.Value.current_state = $STATE_FAULT
                Write-Host "State transition: PRECHARGE -> FAULT (Timeout)"
            }
            # Check if voltage reached threshold
            elseif ($mock_dc_voltage -ge $PRECHARGE_VOLTAGE_THRESHOLD) {
                # Additional check: voltage stable for 500ms
                if (-not $ctrl.Value.voltage_reached) {
                    $ctrl.Value.voltage_reached = $true
                    $ctrl.Value.voltage_stable_time = $current_time
                    Write-Host "Voltage reached threshold, starting stability timer..."
                }
                
                if (($current_time - $ctrl.Value.voltage_stable_time) -ge 500) {
                    $ctrl.Value.current_state = $STATE_RUN
                    $ctrl.Value.voltage_reached = $false
                    $percentage = ($mock_dc_voltage * 100.0) / $DC_BUS_VOLTAGE_MAX
                    Write-Host ("State transition: PRECHARGE -> RUN (Voltage reached {0:F1}%)" -f $percentage)
                }
            } else {
                # Reset stable timer if voltage drops
                if ($ctrl.Value.voltage_reached) {
                    $ctrl.Value.voltage_reached = $false
                    Write-Host "Voltage dropped below threshold, resetting stability timer"
                }
            }
        }
        
        $STATE_RUN {
            # Normal operation - nothing to do here
        }
        
        $STATE_FAULT {
            # Check for reset command
            if ($ctrl.Value.reset_command) {
                $ctrl.Value.current_state = $STATE_IDLE
                $ctrl.Value.reset_command = $false
                Write-Host "State transition: FAULT -> IDLE (Reset)"
            }
        }
    }
}

function state_to_string([int]$state) {
    switch ($state) {
        $STATE_IDLE { return "IDLE" }
        $STATE_PRECHARGE { return "PRECHARGE" }
        $STATE_RUN { return "RUN" }
        $STATE_FAULT { return "FAULT" }
        default { return "UNKNOWN" }
    }
}

function test_normal_sequence {
    Write-Host "`n=== Test Case 1: Normal Precharge Sequence ==="
    
    $ctrl = [PrechargeController]::new()
    precharge_init([ref]$ctrl)
    
    # Simulate normal startup
    $ctrl.start_command = $true
    
    for ($i = 0; $i -lt 20; $i++) {
        # Simulate voltage rising
        $mock_dc_voltage = 50 * $i
        $current_time = $i * 100
        
        precharge_run([ref]$ctrl, $current_time)
        Write-Host ("Time {0}ms: State={1}, Voltage={2}V" -f ($i * 100), (state_to_string $ctrl.current_state), $mock_dc_voltage)
    }
}

function test_timeout_sequence {
    Write-Host "`n=== Test Case 2: Precharge Timeout ==="
    
    $ctrl = [PrechargeController]::new()
    precharge_init([ref]$ctrl)
    
    # Simulate failed precharge (voltage stays low)
    $ctrl.start_command = $true
    $mock_dc_voltage = 100  # Low voltage that won't reach threshold
    
    for ($i = 0; $i -lt 15; $i++) {
        $current_time = $i * 1000
        precharge_run([ref]$ctrl, $current_time)
        Write-Host ("Time {0}ms: State={1}, Voltage={2}V" -f ($i * 1000), (state_to_string $ctrl.current_state), $mock_dc_voltage)
    }
}

function test_fault_and_reset {
    Write-Host "`n=== Test Case 3: Fault and Reset ==="
    
    $ctrl = [PrechargeController]::new()
    precharge_init([ref]$ctrl)
    
    # Start precharge
    $ctrl.start_command = $true
    $mock_dc_voltage = 300
    
    for ($i = 0; $i -lt 5; $i++) {
        $current_time = $i * 100
        precharge_run([ref]$ctrl, $current_time)
        Write-Host ("Time {0}ms: State={1}, Voltage={2}V" -f ($i * 100), (state_to_string $ctrl.current_state), $mock_dc_voltage)
        
        if ($i -eq 2) {
            # Inject fault
            $mock_fault_signal = $true
            Write-Host "*** Fault signal triggered ***"
        }
    }
    
    # Clear fault and reset
    $mock_fault_signal = $false
    $ctrl.reset_command = $true
    
    for ($i = 0; $i -lt 3; $i++) {
        $current_time = 500 + $i * 100
        precharge_run([ref]$ctrl, $current_time)
        Write-Host ("After reset {0}: State={1}" -f $i, (state_to_string $ctrl.current_state))
    }
}

# Run test cases
test_normal_sequence
test_timeout_sequence
test_fault_and_reset

Write-Host "`nAll test cases completed."