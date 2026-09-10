#include <stdio.h>
#include <math.h>

#define MAX_SAMPLES 5000
#define SAMPLE_TIME 0.01

typedef struct {
    double kp;
    double ki;
    double kd;
    double integral;
    double prev_error;
} PIDController;

typedef struct {
    double a;
    double b;
    double prev_output;
} Plant;

typedef struct {
    double ku;
    double tu;
} ZNResult;

void pid_init(PIDController *pid, double kp, double ki, double kd) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->integral = 0.0;
    pid->prev_error = 0.0;
}

double pid_update(PIDController *pid, double error, double dt) {
    pid->integral += error * dt;
    double derivative = (error - pid->prev_error) / dt;
    double output = pid->kp * error + pid->ki * pid->integral + pid->kd * derivative;
    pid->prev_error = error;
    return output;
}

void plant_init(Plant *plant, double a, double b) {
    plant->a = a;
    plant->b = b;
    plant->prev_output = 0.0;
}

double plant_update(Plant *plant, double input) {
    double output = plant->a * plant->prev_output + plant->b * input;
    plant->prev_output = output;
    return output;
}

int detect_oscillation(double *output_history, int count, double *period) {
    if (count < 100) return 0;
    
    int peak_count = 0;
    int peak_indices[10];
    
    for (int i = 50; i < count - 1; i++) {
        if (output_history[i] > output_history[i-1] && output_history[i] > output_history[i+1]) {
            if (peak_count < 10) {
                peak_indices[peak_count++] = i;
            }
        }
    }
    
    if (peak_count < 3) return 0;
    
    double period_sum = 0.0;
    for (int i = 1; i < peak_count; i++) {
        period_sum += (peak_indices[i] - peak_indices[i-1]) * SAMPLE_TIME;
    }
    
    *period = period_sum / (peak_count - 1);
    
    double avg_amplitude = 0.0;
    for (int i = 0; i < peak_count; i++) {
        avg_amplitude += fabs(output_history[peak_indices[i]]);
    }
    avg_amplitude /= peak_count;
    
    double amplitude_var = 0.0;
    for (int i = 0; i < peak_count; i++) {
        double diff = fabs(output_history[peak_indices[i]]) - avg_amplitude;
        amplitude_var += diff * diff;
    }
    amplitude_var = sqrt(amplitude_var / peak_count);
    
    return (amplitude_var / avg_amplitude < 0.05);
}

ZNResult zn_tuning(double plant_a, double plant_b, double setpoint) {
    ZNResult result = {0.0, 0.0};
    double kp_start = 0.1;
    double kp_step = 0.1;
    double kp_max = 20.0;
    
    for (double kp = kp_start; kp <= kp_max; kp += kp_step) {
        PIDController pid;
        Plant plant;
        
        pid_init(&pid, kp, 0.0, 0.0);
        plant_init(&plant, plant_a, plant_b);
        
        double output_history[MAX_SAMPLES];
        int sample_count = 0;
        double output = 0.0;
        
        for (int i = 0; i < MAX_SAMPLES; i++) {
            double error = setpoint - output;
            double control = pid_update(&pid, error, SAMPLE_TIME);
            output = plant_update(&plant, control);
            output_history[sample_count++] = output;
            
            if (i > 300) {
                double period;
                if (detect_oscillation(output_history, sample_count, &period)) {
                    result.ku = kp;
                    result.tu = period;
                    return result;
                }
            }
        }
    }
    
    return result;
}

void print_results(ZNResult result, int test_case) {
    printf("Test Case %d:\n", test_case);
    printf("Critical Gain Ku = %.2f, Critical Period Tu = %.2fs\n", result.ku, result.tu);
    printf("P Controller: Kp=%.2f\n", 0.5 * result.ku);
    printf("PI Controller: Kp=%.2f, Ti=%.2fs\n", 0.45 * result.ku, result.tu / 1.2);
    printf("PID Controller: Kp=%.2f, Ti=%.2fs, Td=%.2fs\n\n", 
           0.6 * result.ku, result.tu / 2.0, result.tu / 8.0);
}

int main() {
    printf("Ziegler-Nichols Critical Gain Method PID Auto-tuning\n");
    printf("====================================================\n\n");
    
    ZNResult result1 = zn_tuning(0.8, 0.2, 1.0);
    print_results(result1, 1);
    
    ZNResult result2 = zn_tuning(0.7, 0.3, 2.0);
    print_results(result2, 2);
    
    return 0;
}