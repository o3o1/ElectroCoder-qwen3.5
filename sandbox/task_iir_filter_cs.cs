using System;

class Program
{
    static float iir_lowpass(float input, float alpha, ref float prev_output)
    {
        float output = alpha * input + (1.0f - alpha) * prev_output;
        prev_output = output;
        return output;
    }

    static void Main()
    {
        const float fs = 1000.0f;      // 采样频率 1000Hz
        const float f_signal = 50.0f;  // 信号频率 50Hz
        const float f_noise = 200.0f;  // 噪声频率 200Hz
        const float alpha = 0.1f;      // 滤波系数
        
        const int num_samples = 100;   // 5个周期，每个周期20个采样点
        float[] signal = new float[num_samples];
        float[] filtered = new float[num_samples];
        
        float prev_output = 0.0f;      // 初始状态 y[-1] = 0
        
        // 生成测试信号：正弦波 + 高频噪声
        for (int i = 0; i < num_samples; i++)
        {
            float t = i / fs;
            signal[i] = (float)(Math.Sin(2.0 * Math.PI * f_signal * t) + 0.3 * Math.Sin(2.0 * Math.PI * f_noise * t));
        }
        
        // 滤波处理
        for (int i = 0; i < num_samples; i++)
        {
            filtered[i] = iir_lowpass(signal[i], alpha, ref prev_output);
        }
        
        // 打印前10个采样点对比
        Console.WriteLine("原始信号和滤波后信号对比（前10个采样点）：");
        for (int i = 0; i < 10; i++)
        {
            Console.WriteLine($"原始信号[{i}]={signal[i]:F6}, 滤波后[{i}]={filtered[i]:F6}");
        }
        
        // 计算峰值变化
        float max_original = 0.0f;
        float max_filtered = 0.0f;
        
        for (int i = 0; i < num_samples; i++)
        {
            float abs_original = Math.Abs(signal[i]);
            float abs_filtered = Math.Abs(filtered[i]);
            
            if (abs_original > max_original) max_original = abs_original;
            if (abs_filtered > max_filtered) max_filtered = abs_filtered;
        }
        
        Console.WriteLine($"\n峰值变化: 原始峰值={max_original:F6}, 滤波后峰值={max_filtered:F6}");
    }
}