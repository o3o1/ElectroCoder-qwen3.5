using System;

public class PLLessSync {
    public float theta;
    public float freq;
    public float u_alpha_prev;
    public float u_beta_prev;
    public float integral;
    public float kp;
    public float ki;
}

public class PowerCalcSim {
    const float PI = 3.14159265358979323846f;
    const float SAMPLE_RATE = 10000.0f;
    const float GRID_FREQ = 50.0f;
    const float GRID_VOLTAGE = 220.0f;
    const float SAMPLE_PERIOD = 1.0f/SAMPLE_RATE;
    const float OMEGA_NOMINAL = 2.0f*PI*GRID_FREQ;
    
    static void ClarkeTransform(float ua, float ub, float uc, out float u_alpha, out float u_beta) {
        u_alpha = (2.0f/3.0f) * (ua - 0.5f*ub - 0.5f*uc);
        u_beta = (2.0f/3.0f) * (0.86602540378f*ub - 0.86602540378f*uc);
    }
    
    static void PLLessSyncUpdate(ref PLLessSync sync, float ua, float ub, float uc) {
        float u_alpha, u_beta;
        ClarkeTransform(ua, ub, uc, out u_alpha, out u_beta);
        
        float q = u_alpha * sync.u_beta_prev - u_beta * sync.u_alpha_prev;
        float u_sq = u_alpha*u_alpha + u_beta*u_beta;
        
        float phase_error = 0.0f;
        if (u_sq > 1e-6f) {
            phase_error = q / u_sq;
        }
        
        float delta_theta = sync.kp * phase_error + sync.ki * sync.integral;
        sync.integral += phase_error * SAMPLE_PERIOD;
        sync.theta += (OMEGA_NOMINAL + delta_theta) * SAMPLE_PERIOD;
        
        while (sync.theta > PI) sync.theta -= 2.0f*PI;
        while (sync.theta < -PI) sync.theta += 2.0f*PI;
        
        sync.freq = GRID_FREQ + delta_theta / (2.0f*PI);
        sync.u_alpha_prev = u_alpha;
        sync.u_beta_prev = u_beta;
    }
    
    static void GenerateThreePhaseVoltage(float time, out float ua, out float ub, out float uc) {
        float omega = 2.0f * PI * GRID_FREQ;
        float phase = omega * time;
        ua = GRID_VOLTAGE * (float)Math.Sin(phase);
        ub = GRID_VOLTAGE * (float)Math.Sin(phase - 2.0f*PI/3.0f);
        uc = GRID_VOLTAGE * (float)Math.Sin(phase + 2.0f*PI/3.0f);
    }
    
    public static void Main() {
        PLLessSync sync = new PLLessSync();
        sync.theta = 0.0f;
        sync.freq = GRID_FREQ;
        sync.u_alpha_prev = 0.0f;
        sync.u_beta_prev = 0.0f;
        sync.integral = 0.0f;
        sync.kp = 100.0f;
        sync.ki = 5000.0f;
        
        int total_samples = (int)(5.0f / GRID_FREQ * SAMPLE_RATE);
        float time = 0.0f;
        float freq_sum = 0.0f;
        int valid_count = 0;
        
        Console.WriteLine("PLL-less Sync Control Simulation");
        Console.WriteLine("Sampling Rate: {0} Hz, Grid Freq: {1} Hz, Voltage: {2} V", 
                         SAMPLE_RATE, GRID_FREQ, GRID_VOLTAGE);
        Console.WriteLine("============================================================");
        Console.WriteLine("Sample\tTime(s)\tPhase(rad)\tFreq(Hz)\tFreqError(Hz)");
        Console.WriteLine("============================================================");
        
        for (int i = 0; i < total_samples; i++) {
            float ua, ub, uc;
            GenerateThreePhaseVoltage(time, out ua, out ub, out uc);
            PLLessSyncUpdate(ref sync, ua, ub, uc);
            float freq_error = sync.freq - GRID_FREQ;
            
            if (i > 100) {
                freq_sum += Math.Abs(freq_error);
                valid_count++;
            }
            
            if (i % 100 == 0) {
                Console.WriteLine("{0}\t{1:F4}\t{2:F4}\t\t{3:F4}\t\t{4:F4}", 
                                 i, time, sync.theta, sync.freq, freq_error);
            }
            
            time += SAMPLE_PERIOD;
        }
        
        float avg_freq_error = (valid_count > 0) ? freq_sum / valid_count : 0.0f;
        Console.WriteLine("============================================================");
        Console.WriteLine("Simulation complete! Processed {0} samples ({1:F1} cycles)", 
                         total_samples, (float)total_samples * GRID_FREQ / SAMPLE_RATE);
        Console.WriteLine("Average frequency error: {0:F6} Hz", avg_freq_error);
        Console.WriteLine("Relative error: {0:F4}%", avg_freq_error / GRID_FREQ * 100.0f);
    }
}