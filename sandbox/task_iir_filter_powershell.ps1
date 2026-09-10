function iir_lowpass {
    param($input_val, $alpha, [ref]$prev_output)
    $output = $alpha * $input_val + (1.0 - $alpha) * $prev_output.Value
    $prev_output.Value = $output
    return $output
}

function main {
    $fs = 1000.0      # 采样频率 1000Hz
    $f_signal = 50.0  # 信号频率 50Hz
    $f_noise = 200.0  # 噪声频率 200Hz
    $alpha = 0.1      # 滤波系数
    
    $num_samples = 100   # 5个周期，每个周期20个采样点
    $signal = New-Object float[] $num_samples
    $filtered = New-Object float[] $num_samples
    
    $prev_output = [ref]0.0  # 初始状态 y[-1] = 0
    
    # 生成测试信号：正弦波 + 高频噪声
    for ($i = 0; $i -lt $num_samples; $i++) {
        $t = $i / $fs
        $signal[$i] = [Math]::Sin(2.0 * [Math]::PI * $f_signal * $t) + 0.3 * [Math]::Sin(2.0 * [Math]::PI * $f_noise * $t)
    }
    
    # 滤波处理
    for ($i = 0; $i -lt $num_samples; $i++) {
        $filtered[$i] = iir_lowpass -input_val $signal[$i] -alpha $alpha -prev_output $prev_output
    }
    
    # 打印前10个采样点对比
    Write-Host "原始信号和滤波后信号对比（前10个采样点）："
    for ($i = 0; $i -lt 10; $i++) {
        Write-Host ("原始信号[{0}]={1:F6}, 滤波后[{0}]={2:F6}" -f $i, $signal[$i], $filtered[$i])
    }
    
    # 计算峰值变化
    $max_original = 0.0
    $max_filtered = 0.0
    
    for ($i = 0; $i -lt $num_samples; $i++) {
        $abs_original = [Math]::Abs($signal[$i])
        $abs_filtered = [Math]::Abs($filtered[$i])
        
        if ($abs_original -gt $max_original) { $max_original = $abs_original }
        if ($abs_filtered -gt $max_filtered) { $max_filtered = $abs_filtered }
    }
    
    Write-Host ("`n峰值变化: 原始峰值={0:F6}, 滤波后峰值={1:F6}" -f $max_original, $max_filtered)
}

main