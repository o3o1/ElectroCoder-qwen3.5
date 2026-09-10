using System;

public class IIRFilter {
    public static float iir_lowpass_filter(float input, ref float prev_output, float alpha) {
        float output = alpha * input + (1.0f - alpha) * prev_output;
        prev_output = output;
        return output;
    }
    
    public static void Main() {
        const int N = 15;
        float[] original_signal = new float[N];
        
        for (int i = 0; i < N; i++) {
            float t = i * 0.1f;
            float sine_wave = (float)Math.Sin(2.0 * Math.PI * 1.0 * t);
            float noise = 0.3f * (float)Math.Sin(2.0 * Math.PI * 10.0 * t);
            original_signal[i] = sine_wave + noise;
        }
        
        float[] alphas = {0.1f, 0.5f, 0.9f};
        const int num_alphas = 3;
        float[,] filtered_signals = new float[num_alphas, N];
        
        for (int a = 0; a < num_alphas; a++) {
            float prev_output = 0.0f;
            for (int i = 0; i < N; i++) {
                filtered_signals[a, i] = iir_lowpass_filter(original_signal[i], ref prev_output, alphas[a]);
            }
        }
        
        Console.WriteLine("Sample | Original | alpha=0.1 | alpha=0.5 | alpha=0.9");
        Console.WriteLine("--------------------------------------------------------");
        
        for (int i = 0; i < N; i++) {
            Console.WriteLine("  {0,2}   | {1,7:F3}  | {2,8:F3}   | {3,8:F3}   | {4,8:F3}",
                   i,
                   original_signal[i],
                   filtered_signals[0, i],
                   filtered_signals[1, i],
                   filtered_signals[2, i]);
        }
        
        Console.WriteLine("\n=== Filter Analysis ===");
        Console.WriteLine("alpha=0.1: Strongest filtering, best noise suppression, slowest response");
        Console.WriteLine("alpha=0.5: Balanced, moderate noise suppression, medium response");
        Console.WriteLine("alpha=0.9: Weakest filtering, follows original signal, fastest response");
    }
}