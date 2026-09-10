#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define TEMP_SENSOR_REG (*((volatile float*)0x40000000))
#define INVERTER_FREQ_REG (*((volatile float*)0x40000004))

#define PID_KP 2.0f
#define PID_KI 0.1f
#define PID_KD 1.0f
#define TEMP_DEADBAND 0.5f
#define SAMPLE_PERIOD 1.0f
#define MIN_FREQ 0.0f
#define MAX_FREQ 50.0f
#define SIMULATION_TIME 60

typedef struct {
    float Kp;
    float Ki;
    float Kd;
    float integral;
    float prev_error;
    float output_min;
    float output_max;
} PIDController;

void pid_init(PIDController* pid, float Kp, float Ki, float Kd, float min, float max) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
    pid->output_min = min;
    pid->output_max = max;
}

float pid_update(PIDController* pid, float setpoint, float measurement, float dt) {
    float error = setpoint - measurement;
    
    if (fabsf(error) <= TEMP_DEADBAND) {
        pid->integral = 0.0f;
        pid->prev_error = error;
        return 0.0f;
    }
    
    pid->integral += error * dt;
    
    float derivative = (error - pid->prev_error) / dt;
    
    float output = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * derivative;
    
    if (output > pid->output_max) output = pid->output_max;
    if (output < pid->output_min) output = pid->output_min;
    
    pid->prev_error = error;
    
    return output;
}

float mock_read_temperature(float setpoint, float current_temp, float output_freq, int time_step) {
    static float last_temp = 25.0f;
    
    if (time_step == 0) {
        return last_temp;
    }
    
    float temp_change = 0.0f;
    
    if (output_freq > 0.0f) {
        if (setpoint < current_temp) {
            temp_change = -0.1f * output_freq / 50.0f;
        } else {
            temp_change = 0.08f * output_freq / 50.0f;
        }
    }
    
    float ambient_influence = 0.0f;
    if (time_step > 20 && time_step < 40) {
        ambient_influence = 0.05f;
    }
    
    float noise = ((rand() % 100) - 50) / 1000.0f;
    
    last_temp = current_temp + temp_change + ambient_influence + noise;
    
    if (last_temp < 15.0f) last_temp = 15.0f;
    if (last_temp > 35.0f) last_temp = 35.0f;
    
    TEMP_SENSOR_REG = last_temp;
    return last_temp;
}

void mock_set_inverter_frequency(float freq) {
    INVERTER_FREQ_REG = freq;
}

void run_test_scenario(float initial_temp, float setpoint, const char* scenario_name, int* stable_time, float* overshoot) {
    printf("\n=== %s ===\n", scenario_name);
    printf("Initial temp: %.1fC, Setpoint: %.1fC\n\n", initial_temp, setpoint);
    printf("Time(s)  Setpoint  Actual    Error    Freq(Hz)\n");
    printf("--------------------------------------------\n");
    
    PIDController pid;
    pid_init(&pid, PID_KP, PID_KI, PID_KD, MIN_FREQ, MAX_FREQ);
    
    float current_temp = initial_temp;
    int is_stable = 0;
    *stable_time = 0;
    *overshoot = 0.0f;
    float max_overshoot = 0.0f;
    int stable_start = -1;
    
    for (int t = 0; t < SIMULATION_TIME; t++) {
        float error = setpoint - current_temp;
        float output_freq = pid_update(&pid, setpoint, current_temp, SAMPLE_PERIOD);
        
        mock_set_inverter_frequency(output_freq);
        
        printf("%6d   %7.1f   %7.1f   %6.2f   %8.1f\n", 
               t, setpoint, current_temp, error, output_freq);
        
        if (fabsf(error) <= 0.2f) {
            if (!is_stable) {
                is_stable = 1;
                stable_start = t;
            }
        } else {
            is_stable = 0;
            stable_start = -1;
        }
        
        if (error * (setpoint - initial_temp) < 0) {
            float current_overshoot = fabsf(error);
            if (current_overshoot > max_overshoot) {
                max_overshoot = current_overshoot;
            }
        }
        
        current_temp = mock_read_temperature(setpoint, current_temp, output_freq, t);
        
        if (t == SIMULATION_TIME - 1 && stable_start != -1) {
            *stable_time = stable_start;
        }
    }
    
    *overshoot = max_overshoot;
    
    printf("\nStatistics:\n");
    printf("Stable time: %d seconds\n", *stable_time);
    printf("Max overshoot: %.2fC\n", *overshoot);
    printf("Final error: %.2fC\n", setpoint - current_temp);
}

int main() {
    srand(time(NULL));
    
    printf("Central Air Conditioner Inverter Temperature Control Simulation\n");
    printf("===============================================================\n");
    
    int stable_time1, stable_time2;
    float overshoot1, overshoot2;
    
    run_test_scenario(25.0f, 22.0f, "Test A: Cooling (25C->22C)", &stable_time1, &overshoot1);
    
    run_test_scenario(18.0f, 22.0f, "Test B: Heating (18C->22C)", &stable_time2, &overshoot2);
    
    printf("\n=== Test C: Temperature Fluctuation ===\n");
    printf("Phase 1: 25C->22C, Phase 2: 22C->24C, Phase 3: 24C->22C\n\n");
    
    PIDController pid;
    pid_init(&pid, PID_KP, PID_KI, PID_KD, MIN_FREQ, MAX_FREQ);
    
    float current_temp = 25.0f;
    float setpoint = 22.0f;
    int phase = 1;
    
    printf("Time(s)  Setpoint  Actual    Error    Freq(Hz)  Phase\n");
    printf("---------------------------------------------------\n");
    
    for (int t = 0; t < SIMULATION_TIME; t++) {
        if (t == 20) {
            setpoint = 24.0f;
            phase = 2;
            pid.integral = 0.0f;
        } else if (t == 40) {
            setpoint = 22.0f;
            phase = 3;
            pid.integral = 0.0f;
        }
        
        float error = setpoint - current_temp;
        float output_freq = pid_update(&pid, setpoint, current_temp, SAMPLE_PERIOD);
        
        mock_set_inverter_frequency(output_freq);
        
        printf("%6d   %7.1f   %7.1f   %6.2f   %8.1f   %d\n", 
               t, setpoint, current_temp, error, output_freq, phase);
        
        current_temp = mock_read_temperature(setpoint, current_temp, output_freq, t);
    }
    
    printf("\n=== Test Summary ===\n");
    printf("Test A (Cooling): Stable time=%ds, Overshoot=%.2fC\n", stable_time1, overshoot1);
    printf("Test B (Heating): Stable time=%ds, Overshoot=%.2fC\n", stable_time2, overshoot2);
    printf("Test C (Fluctuation): Multi-phase tracking completed\n");
    
    return 0;
}