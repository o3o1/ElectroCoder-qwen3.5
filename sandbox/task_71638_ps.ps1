$NUM_CHANNELS = 3
$SAMPLES_PER_CHANNEL = 4
$BUFFER_SIZE = $NUM_CHANNELS * $SAMPLES_PER_CHANNEL
$SAMPLE_RATE_HZ = 1000

$adc_buffer = New-Object int[] $BUFFER_SIZE
$buffer_index = 0
$sample_count = 0
$stop_sampling = $false

function ADC_Init {
    Write-Host "ADC initialized (3 channels: PA0, PA1, PA2)"
}

function DMA_Init {
    Write-Host "DMA initialized (circular buffer size: $BUFFER_SIZE)"
}

function ADC_Start {
    Write-Host "ADC started with DMA transfer"
}

function DMA_IRQHandler {
    $script:sample_count++
    Write-Host "DMA Callback - Buffer ready (cycle $sample_count)"
    
    if ($sample_count -ge 5) {
        $script:stop_sampling = $true
    }
}

function ADC_SimulateConversion {
    $random = New-Object System.Random
    
    for ($ch = 0; $ch -lt $NUM_CHANNELS; $ch++) {
        $adc_buffer[$script:buffer_index] = $random.Next(0, 4096)
        $script:buffer_index++
        
        if ($script:buffer_index -ge $BUFFER_SIZE) {
            $script:buffer_index = 0
            DMA_IRQHandler
        }
    }
}

function Print_Channel_Samples {
    Write-Host "`nFinal ADC Samples:"
    
    for ($ch = 0; $ch -lt $NUM_CHANNELS; $ch++) {
        $samples = @()
        for ($i = 0; $i -lt $SAMPLES_PER_CHANNEL; $i++) {
            $idx = ($BUFFER_SIZE - ($NUM_CHANNELS - $ch) - ($i * $NUM_CHANNELS))
            if ($idx -lt 0) { $idx += $BUFFER_SIZE }
            $samples += $adc_buffer[$idx]
        }
        
        Write-Host "Channel $($ch+1) samples: [$($samples -join ', ')]"
    }
}

# Main execution
Write-Host "STM32 ADC DMA Simulation Started"
Write-Host "================================"

ADC_Init
DMA_Init
ADC_Start

Write-Host "`nStarting data acquisition (5 cycles)..."

while (-not $stop_sampling) {
    ADC_SimulateConversion
    Start-Sleep -Milliseconds 1
}

Write-Host "`nData acquisition completed ($sample_count cycles)"
Print_Channel_Samples