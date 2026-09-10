using System;

public class PIDTest {
    public struct PID_Controller {
        public float Kp;
        public float Ki;
        public float Kd;
        public float integral;
        public float prev_error;
        public float integral_limit;
        public float output_limit;
        public float error_threshold;
    }
    
    public static void PID_Init(ref PID_Controller pid, float kp, float ki, float kd, float int_limit, float out_limit, float err_thresh) {
        pid.Kp = kp;
        pid.Ki = ki;
        pid.Kd = kd;
        pid.integral = 0.0f;
        pid.prev_error = 0.0f;
        pid.integral_limit = int_limit;
        pid.output_limit = out_limit;
        pid.error_threshold = err_thresh;
    }
    
    public static float PID_Update(ref PID_Controller pid, float setpoint, float measurement, float dt) {
        float error = setpoint - measurement;
        
        if (Math.Abs(error) > pid.error_threshold) {
            pid.integral = 0.0f;
        } else {
            float integral_update = pid.Ki * error * dt;
            float new_integral = pid.integral + integral_update;
            
            if (Math.Abs(new_integral) <= pid.integral_limit) {
                pid.integral = new_integral;
            }
        }
        
        float derivative = (error - pid.prev_error) / dt;
        pid.prev_error = error;
        
        float output = pid.Kp * error + pid.integral + pid.Kd * derivative;
        
        if (output > pid.output_limit) {
            output = pid.output_limit;
        } else if (output < -pid.output_limit) {
            output = -pid.output_limit;
        }
        
        return output;
    }
    
    public static void Main() {
        PID_Controller pid = new PID_Controller();
        PID_Init(ref pid, 1.0f, 0.5f, 0.1f, 2.0f, 5.0f, 3.0f);
        
        float[] setpoints = {10.0f, 10.0f, 10.0f, 10.0f, 10.0f};
        float[] measurements = {0.0f, 2.0f, 4.0f, 8.0f, 12.0f};
        float dt = 0.1f;
        
        Console.WriteLine("PID Anti-Windup Controller Test");
        Console.WriteLine($"Kp={pid.Kp:F1}, Ki={pid.Ki:F1}, Kd={pid.Kd:F1}, IntLimit={pid.integral_limit:F1}, OutLimit={pid.output_limit:F1}, ErrThresh={pid.error_threshold:F1}");
        Console.WriteLine();
        
        for (int i = 0; i < 5; i++) {
            float output = PID_Update(ref pid, setpoints[i], measurements[i], dt);
            Console.WriteLine($"Cycle {i+1}: Setpoint={setpoints[i]:F1}, Measurement={measurements[i]:F1}, Error={setpoints[i]-measurements[i]:F1}, Output={output:F3}, Integral={pid.integral:F3}");
        }
    }
}