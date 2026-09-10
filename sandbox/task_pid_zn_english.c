#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define MAX_SAMPLES 10000
#define SAMPLE_TIME 0.01
#define AMPLITUDE_TOLERANCE 0.01
#define MAX_OSCILLATIONS 10

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

int detect_oscillation(double *output_history, int count, double *amplitude, double *period) {
    if (count < 100) return 0;
    
    double max_val = -1e9;
    double min_val = 1e9;
    int max_indices[20];
    int min_indices[20];
    int max_count = 0;
    int min_count = 0;
    
    for (int i = 50; i < count - 1; i++) {
        if (output_history[i] > output_history[i-1] && output_history[i] > output_history[i+1]) {
            if (max_count < 20) {
                max_indices[max_count++] = i;
                if (output_history[i] > max_val) max_val = output_history[i];
            }
        }
        if (output_history[i] < output_history[i-1] && output_history[i] < output_history[i+1]) {
            if (min_count < 20) {
                min_indices[min_count++] = i;
                if (output_history[i] < min_val) min_val = output_history[i];
            }
        }
    }
    
    if (max_count < 3 || min_count < 3) return 0;
    
    *amplitude = (max_val - min_val) / 2.0;
    
    double period_sum = 0.0;
    int period_count = 0;
    
    for (int i = 1; i < max_count; i++) {
        double p = (max_indices[i] - max_indices[i-1]) * SAMPLE_TIME;
        if (p > 0.1) {
            period_sum += p;
            period_count++;
        }
    }
    
    if (period_count < 2) return 0;
    
    *period = period_sum / period_count;
    
    double amplitude_std = 0.0;
    double last_peak = fabs(output_history[max_indices[0]]);
    for (int i = 1; i < max_count; i++) {
        double current_peak = fabs(output_history[max_indices[i]]);
        amplitude_std += fabs(current_peak - last_peak);
        last_peak = current_peak;
    }
    amplitude_std /= (max_count - 1);
    
    return (amplitude_std / *amplitude < AMPLITUDE_TOLERANCE);
}

ZNResult zn_tuning(double plant_a, double plant_b, double setpoint) {
    ZNResult result = {0.0, 0.0};
    double kp_start = 0.1;
    double kp_step = 0.1;
    double kp_max = 50.0;
    
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
            
            if (i > 500) {
                double amplitude, period;
                if (detect_oscillation(output_history, sample_count, &amplitude, &period)) {
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
    
    double plant_a = 0.8;
    double plant_b = 0.2;
    
    ZNResult result1 = zn_tuning(plant_a, plant_b, 1.0);
    print_results(result1, 1);
    
    ZNResult result2 = zn_tuning(0.7, 0.3, 2.0);
    print_results(result2, 2);
    
    return 0;
}