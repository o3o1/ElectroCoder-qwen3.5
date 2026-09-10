function iir_lowpass(input, alpha, prev_output) {
    let output = alpha * input + (1.0 - alpha) * prev_output.value;
    prev_output.value = output;
    return output;
}

function main() {
    const fs = 1000.0;      // 采样频率 1000Hz
    const f_signal = 50.0;  // 信号频率 50Hz
    const f_noise = 200.0;  // 噪声频率 200Hz
    const alpha = 0.1;      // 滤波系数
    
    const num_samples = 100;   // 5个周期，每个周期20个采样点
    let signal = new Array(num_samples);
    let filtered = new Array(num_samples);
    
    let prev_output = { value: 0.0 };  // 初始状态 y[-1] = 0
    
    // 生成测试信号：正弦波 + 高频噪声
    for (let i = 0; i < num_samples; i++) {
        let t = i / fs;
        signal[i] = Math.sin(2.0 * Math.PI * f_signal * t) + 0.3 * Math.sin(2.0 * Math.PI * f_noise * t);
    }
    
    // 滤波处理
    for (let i = 0; i < num_samples; i++) {
        filtered[i] = iir_lowpass(signal[i], alpha, prev_output);
    }
    
    // 打印前10个采样点对比
    console.log("原始信号和滤波后信号对比（前10个采样点）：");
    for (let i = 0; i < 10; i++) {
        console.log(`原始信号[${i}]=${signal[i].toFixed(6)}, 滤波后[${i}]=${filtered[i].toFixed(6)}`);
    }
    
    // 计算峰值变化
    let max_original = 0.0;
    let max_filtered = 0.0;
    
    for (let i = 0; i < num_samples; i++) {
        let abs_original = Math.abs(signal[i]);
        let abs_filtered = Math.abs(filtered[i]);
        
        if (abs_original > max_original) max_original = abs_original;
        if (abs_filtered > max_filtered) max_filtered = abs_filtered;
    }
    
    console.log(`\n峰值变化: 原始峰值=${max_original.toFixed(6)}, 滤波后峰值=${max_filtered.toFixed(6)}`);
}

main();