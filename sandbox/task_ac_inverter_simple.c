#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define PID_KP 2.0
#define PID_KI 0.1
#define PID_KD 1.0
#define TEMP_DEADBAND 0.5
#define SAMPLE_PERIOD 1.0
#define MIN_FREQ 0.0
#define MAX_FREQ 50.0
#define SIMULATION_TIME 60

typedef struct {
    double Kp;
    double Ki;
    double Kd;
    double integral;
    double prev_error;
    double output_min;
    double output_max;
} PIDController;

void pid_init(PIDController* pid, double Kp, double Ki, double Kd, double min, double max) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->integral = 0.0;
    pid->prev_error = 0.0;
    pid->output_min = min;
    pid->output_max = max;
}

double pid_update(PIDController* pid, double setpoint, double measurement, double dt) {
    double error = setpoint - measurement;
    
    if (fabs(error) <= TEMP_DEADBAND) {
        pid->integral = 0.0;
        pid->prev_error = error;
        return 0.0;
    }
    
    pid->integral += error * dt;
    
    double derivative = (error - pid->prev_error) / dt;
    
    double output = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * derivative;
    
    if (output > pid->output_max) output = pid->output_max;
    if (output < pid->output_min) output = pid->output_min;
    
    pid->prev_error = error;
    
    return output;
}

double mock_read_temperature(double setpoint, double current_temp, double output_freq, int time_step) {
    static double last_temp = 25.0;
    
    if (time_step == 0) {
        return last_temp;
    }
    
    double temp_change = 0.0;
    
    if (output_freq > 0) {
        if (setpoint < current_temp) {
            temp_change = -0.1 * output_freq / 50.0;
        } else {
            temp_change = 0.08 * output_freq / 50.0;
        }
    }
    
    double ambient_influence = 0.0;
    if (time_step > 20 && time_step < 40) {
        ambient_influence = 0.05;
    }
    
    double noise = ((rand() % 100) - 50) / 1000.0;
    
    last_temp = current_temp + temp_change + ambient_influence + noise;
    
    if (last_temp < 15.0) last_temp = 15.0;
    if (last_temp > 35.0) last_temp = 35.0;
    
    return last_temp;
}

void run_test_scenario(double initial_temp, double setpoint, const char* scenario_name) {
    printf("\n=== %s ===\n", scenario_name);
    printf("初始温度: %.1fC, 设定温度: %.1fC\n\n", initial_temp, setpoint);
    printf("时间(s)  设定温度  实际温度  误差    频率(Hz)\n");
    printf("--------------------------------------------\n");
    
    PIDController pid;
    pid_init(&pid, PID_KP, PID_KI, PID_KD, MIN_FREQ, MAX_FREQ);
    
    double current_temp = initial_temp;
    int stable_time = 0;
    double max_overshoot = 0.0;
    int is_stable = 0;
    int stable_start = -1;
    
    for (int t = 0; t < SIMULATION_TIME; t++) {
        double error = setpoint - current_temp;
        double output_freq = pid_update(&pid, setpoint, current_temp, SAMPLE_PERIOD);
        
        printf("%6d   %7.1f   %7.1f   %6.2f   %8.1f\n", 
               t, setpoint, current_temp, error, output_freq);
        
        if (fabs(error) <= 0.2) {
            if (!is_stable) {
                is_stable = 1;
                stable_start = t;
            }
        } else {
            is_stable = 0;
            stable_start = -1;
        }
        
        if (error * (setpoint - initial_temp) < 0) {
            double current_overshoot = fabs(error);
            if (current_overshoot > max_overshoot) {
                max_overshoot = current_overshoot;
            }
        }
        
        current_temp = mock_read_temperature(setpoint, current_temp, output_freq, t);
        
        if (t == SIMULATION_TIME - 1 && stable_start != -1) {
            stable_time = stable_start;
        }
    }
    
    printf("\n统计信息:\n");
    printf("稳定时间: %d秒\n", stable_time);
    printf("最大超调量: %.2fC\n", max_overshoot);
    printf("最终误差: %.2fC\n", setpoint - current_temp);
}

int main() {
    srand(time(NULL));
    
    printf("中央空调变频器温度闭环控制模拟\n");
    printf("================================\n");
    
    run_test_scenario(25.0, 22.0, "测试场景A: 降温过程(25C->22C)");
    
    run_test_scenario(18.0, 22.0, "测试场景B: 升温过程(18C->22C)");
    
    printf("\n=== 测试场景C: 温度波动场景 ===\n");
    printf("阶段1: 25C->22C, 阶段2: 22C->24C, 阶段3: 24C->22C\n\n");
    
    PIDController pid;
    pid_init(&pid, PID_KP, PID_KI, PID_KD, MIN_FREQ, MAX_FREQ);
    
    double current_temp = 25.0;
    double setpoint = 22.0;
    int phase = 1;
    
    printf("时间(s)  设定温度  实际温度  误差    频率(Hz)  阶段\n");
    printf("---------------------------------------------------\n");
    
    for (int t = 0; t < SIMULATION_TIME; t++) {
        if (t == 20) {
            setpoint = 24.0;
            phase = 2;
            pid.integral = 0.0;
        } else if (t == 40) {
            setpoint = 22.0;
            phase = 3;
            pid.integral = 0.0;
        }
        
        double error = setpoint - current_temp;
        double output_freq = pid_update(&pid, setpoint, current_temp, SAMPLE_PERIOD);
        
        printf("%6d   %7.1f   %7.1f   %6.2f   %8.1f   %d\n", 
               t, setpoint, current_temp, error, output_freq, phase);
        
        current_temp = mock_read_temperature(setpoint, current_temp, output_freq, t);
    }
    
    printf("\n=== 测试完成 ===\n");
    
    return 0;
}