using System;

namespace PLLSimulation
{
    public class PLL_State
    {
        public double v_alpha;
        public double v_beta;
        public double v_alpha_z1;
        public double v_beta_z1;
        public double vd;
        public double vq;
        public double freq;
        public double freq_hz;
        public double theta;
        public double pi_integral;
        
        public PLL_State()
        {
            const double PI = Math.PI;
            const double GRID_FREQ = 50.0;
            
            v_alpha = 0.0;
            v_beta = 0.0;
            v_alpha_z1 = 0.0;
            v_beta_z1 = 0.0;
            vd = 0.0;
            vq = 0.0;
            freq = 2.0 * PI * GRID_FREQ;
            freq_hz = GRID_FREQ;
            theta = 0.0;
            pi_integral = 0.0;
        }
    }
    
    public class PLLSimulator
    {
        private const double PI = Math.PI;
        private const double SAMPLE_FREQ = 10000.0;
        private const double GRID_FREQ = 50.0;
        private const double SAMPLE_TIME = 1.0 / SAMPLE_FREQ;
        private const double SOGI_K = 0.707;
        private const double PI_KP = 100.0;
        private const double PI_KI = 2000.0;
        
        public static void pll_init(PLL_State pll)
        {
            pll.v_alpha = 0.0;
            pll.v_beta = 0.0;
            pll.v_alpha_z1 = 0.0;
            pll.v_beta_z1 = 0.0;
            pll.vd = 0.0;
            pll.vq = 0.0;
            pll.freq = 2.0 * PI * GRID_FREQ;
            pll.freq_hz = GRID_FREQ;
            pll.theta = 0.0;
            pll.pi_integral = 0.0;
        }
        
        public static void pll_update(PLL_State pll, double v_in)
        {
            double omega = pll.freq;
            double k = SOGI_K;
            double Ts = SAMPLE_TIME;
            
            double v_alpha_new = pll.v_alpha_z1 + k * omega * Ts * (v_in - pll.v_alpha_z1) - omega * Ts * pll.v_beta_z1;
            double v_beta_new = pll.v_beta_z1 + omega * Ts * pll.v_alpha_z1;
            
            pll.v_alpha = v_alpha_new;
            pll.v_beta = v_beta_new;
            pll.v_alpha_z1 = v_alpha_new;
            pll.v_beta_z1 = v_beta_new;
            
            double sin_theta = Math.Sin(pll.theta);
            double cos_theta = Math.Cos(pll.theta);
            
            pll.vd = pll.v_alpha * cos_theta + pll.v_beta * sin_theta;
            pll.vq = -pll.v_alpha * sin_theta + pll.v_beta * cos_theta;
            
            double error = pll.vq;
            
            pll.pi_integral += PI_KI * Ts * error;
            double freq_delta = PI_KP * error + pll.pi_integral;
            
            pll.freq = 2.0 * PI * GRID_FREQ + freq_delta;
            pll.freq_hz = pll.freq / (2.0 * PI);
            
            pll.theta += pll.freq * Ts;
            if (pll.theta > 2.0 * PI)
            {
                pll.theta -= 2.0 * PI;
            }
            if (pll.theta < 0)
            {
                pll.theta += 2.0 * PI;
            }
        }
        
        public static void Main()
        {
            PLL_State pll = new PLL_State();
            pll_init(pll);
            
            Console.WriteLine("Single-phase PLL Algorithm Test - Based on SOGI");
            Console.WriteLine($"Sampling frequency: {SAMPLE_FREQ:F0} Hz, Grid frequency: {GRID_FREQ:F1} Hz");
            Console.WriteLine("Time(s)\tInput Voltage\tEstimated Freq(Hz)\tEstimated Phase(rad)");
            Console.WriteLine(new string('-', 60));
            
            double time = 0.0;
            double final_freq_error = 0.0;
            double final_phase_error = 0.0;
            
            for (int i = 0; i < 1000; i++)
            {
                double v_in = Math.Sin(2.0 * PI * GRID_FREQ * time);
                
                pll_update(pll, v_in);
                
                if (i % 100 == 0)
                {
                    Console.WriteLine($"{time:F4}\t{v_in:F6}\t{pll.freq_hz:F6}\t{pll.theta:F6}");
                }
                
                time += SAMPLE_TIME;
                
                if (i == 999)
                {
                    double expected_phase = (2.0 * PI * GRID_FREQ * time) % (2.0 * PI);
                    final_freq_error = pll.freq_hz - GRID_FREQ;
                    final_phase_error = pll.theta - expected_phase;
                    if (final_phase_error > PI) final_phase_error -= 2.0 * PI;
                    if (final_phase_error < -PI) final_phase_error += 2.0 * PI;
                }
            }
            
            Console.WriteLine("\nFinal Results:");
            Console.WriteLine($"Estimated frequency: {pll.freq_hz:F6} Hz");
            Console.WriteLine($"Frequency error: {final_freq_error:F6} Hz");
            Console.WriteLine($"Phase error: {final_phase_error:F6} rad ({final_phase_error * 180.0 / PI:F6} degrees)");
        }
    }
}