Write-Host "并网逆变器频率自适应控制测试" -ForegroundColor Green
Write-Host "==========================================" -ForegroundColor Cyan

class GridInverter {
    [float]$current_freq
    [float]$target_freq
    [bool]$is_locked
    [float]$phase_error
    
    GridInverter() {
        $this.current_freq = 0.0
        $this.target_freq = 0.0
        $this.is_locked = $false
        $this.phase_error = 0.0
    }
}

function Detect-GridFrequency([GridInverter]$inv, [float]$measured_freq) {
    $inv.current_freq = $measured_freq
    $inv.is_locked = ($measured_freq -ge 45.0 -and $measured_freq -le 55.0)
}

function Adaptive-Control([GridInverter]$inv) {
    if (-not $inv.is_locked) {
        $inv.target_freq = 0.0
        return
    }
    
    $freq = $inv.current_freq
    
    if ($freq -ge 49.5 -and $freq -le 50.5) {
        $inv.target_freq = 50.0
    } elseif ($freq -ge 45.0 -and $freq -lt 49.5) {
        $inv.target_freq = 50.0 + (50.0 - $freq) * 0.3
    } elseif ($freq -gt 50.5 -and $freq -le 55.0) {
        $inv.target_freq = 50.0 - ($freq - 50.0) * 0.3
    }
    
    if ($inv.target_freq -lt 45.0) { $inv.target_freq = 45.0 }
    if ($inv.target_freq -gt 55.0) { $inv.target_freq = 55.0 }
}

function Update-Phase([GridInverter]$inv) {
    if (-not $inv.is_locked) {
        $inv.phase_error = 0.0
        return
    }
    $inv.phase_error = ($inv.target_freq - $inv.current_freq) * 10.0
}

# 测试用例
$inverter = [GridInverter]::new()
$test_cases = @(49.0, 50.2, 52.5, 44.0)

for ($i = 0; $i -lt $test_cases.Count; $i++) {
    Write-Host "`n测试用例 $($i+1): 测量频率 $($test_cases[$i])Hz" -ForegroundColor Yellow
    
    Detect-GridFrequency $inverter $test_cases[$i]
    Adaptive-Control $inverter
    Update-Phase $inverter
    
    Write-Host "  当前频率: $($inverter.current_freq.ToString('F2')) Hz"
    Write-Host "  目标频率: $($inverter.target_freq.ToString('F2')) Hz"
    Write-Host "  锁定状态: $(if ($inverter.is_locked) {'锁定'} else {'未锁定'})"
    Write-Host "  相位误差: $($inverter.phase_error.ToString('F2')) 度"
}

Write-Host "`n测试完成！" -ForegroundColor Green