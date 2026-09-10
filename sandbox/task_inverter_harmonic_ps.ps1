$PI = [Math]::PI

class RepetitiveController {
    [int]$write_idx = 0
    [double]$prev_output = 0.0
    
    [double] Control([double[]]$error_buffer, [int]$buffer_len, [double]$k_r) {
        $current_error = $error_buffer[$this.write_idx]
        $new_output = $k_r * ($this.prev_output + $current_error) + (1 - $k_r) * $this.prev_output
        
        $this.prev_output = $new_output
        $this.write_idx = ($this.write_idx + 1) % $buffer_len
        
        return $new_output
    }
}

function Main {
    $fs = 10000.0
    $f0 = 50.0
    $buffer_len = [Math]::Truncate($fs / $f0)
    
    $error_buffer = New-Object double[] $buffer_len
    $Ts = 1.0 / $fs
    
    $controller = [RepetitiveController]::new()
    $write_idx = 0
    
    for ($cycle = 0; $cycle -lt 5; $cycle++) {
        $max_output = -1e9
        $min_output = 1e9
        
        for ($i = 0; $i -lt $buffer_len; $i++) {
            $t = ($cycle * $buffer_len + $i) * $Ts
            $error = 0.1 * [Math]::Sin(2 * $PI * 50 * $t) + 0.05 * [Math]::Sin(2 * $PI * 150 * $t)
            
            $error_buffer[$write_idx] = $error
            $output = $controller.Control($error_buffer, $buffer_len, 0.5)
            
            if ($output -gt $max_output) {
                $max_output = $output
            }
            if ($output -lt $min_output) {
                $min_output = $output
            }
            
            $write_idx = ($write_idx + 1) % $buffer_len
        }
        
        Write-Host "Cycle $($cycle + 1): Max output = $($max_output.ToString('F6')), Min output = $($min_output.ToString('F6'))"
    }
}

Main