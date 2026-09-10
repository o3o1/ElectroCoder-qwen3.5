using System;

public class FuzzyPID {
    public float kp, ki, kd;
    public float error, last_error;
    public float integral, derivative;
    public float temperature;
    public float[,] rule_base;
    
    public FuzzyPID() {
        kp = 1.0f;
        ki = 0.1f;
        kd = 0.05f;
        error = 0.0f;
        last_error = 0.0f;
        integral = 0.0f;
        derivative = 0.0f;
        temperature = 25.0f;
        rule_base = new float[5,5] {
            {-0.2f, -0.15f, -0.1f, -0.05f, 0.0f},
            {-0.15f, -0.1f, -0.05f, 0.0f, 0.05f},
            {-0.1f, -0.05f, 0.0f, 0.05f, 0.1f},
            {-0.05f, 0.0f, 0.05f, 0.1f, 0.15f},
            {0.0f, 0.05f, 0.1f, 0.15f, 0.2f}
        };
    }
    
    public float FuzzyInference(float error, float d_error, float temperature) {
        float e_norm = Math.Max(-2.0f, Math.Min(2.0f, error));
        float de_norm = Math.Max(-2.0f, Math.Min(2.0f, d_error));
        
        int e_idx = (int)((e_norm + 2.0f) / 4.0f * 4);
        int de_idx = (int)((de_norm + 2.0f) / 4.0f * 4);
        
        e_idx = Math.Max(0, Math.Min(4, e_idx));
        de_idx = Math.Max(0, Math.Min(4, de_idx));
        
        float base_adjust = rule_base[e_idx, de_idx];
        float temp_factor = 1.0f + (temperature - 25.0f) / 100.0f;
        
        return base_adjust * temp_factor;
    }
    
    public float Update(float setpoint, float measured, float dt) {
        last_error = error;
        error = setpoint - measured;
        
        float d_error = (error - last_error) / dt;
        
        float adjust = FuzzyInference(error, d_error, temperature);
        
        float kp_adj = kp * (1.0f + adjust);
        float ki_adj = ki * (1.0f + adjust * 0.5f);
        float kd_adj = kd * (1.0f + adjust * 0.3f);
        
        integral += error * dt;
        derivative = d_error;
        
        float output = kp_adj * error + ki_adj * integral + kd_adj * derivative;
        
        return output;
    }
}

public class Program {
    public static void Main() {
        FuzzyPID pid = new FuzzyPID();
        
        float setpoint = 5.0f;
        float measured = 0.0f;
        float dt = 0.1f;
        
        for (int i = 0; i < 5; i++) {
            float time = (i + 1) * dt;
            pid.temperature = 25.0f + i * 15.0f;
            
            float output = pid.Update(setpoint, measured, dt);
            
            measured += output * dt * 0.1f;
            
            Console.WriteLine("Time=" + time.ToString("F1") + "s, Temp=" + pid.temperature.ToString("F1") + "°C, Output=" + output.ToString("F2") + "V");
        }
    }
}