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
#define SIMULATION_TIME 300

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
    
    if (fabs(error) <= TEMP_DEADBAND) {
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
    
    if (output_freq > 0) {
        if (setpoint < current_temp) {
            temp_change = -0.1f * output_freq / 50.0f;
        } else {
            temp_change = 0.08f * output_freq / 50.0f;
        }
    }
    
    float ambient_influence = 0.0f;
    if (time_step > 100 && time_step < 150) {
        ambient_influence = 0.05f;
    }
    
    float noise = ((rand() % 100) - 50) / 1000.0f;
    
    last_temp = current_temp + temp_change + ambient_influence + noise;
    
    if (last_temp < 15.0f) last_temp = 15.0f;
    if (last_temp > 35.0f) last_temp = 35.0f;
    
    return last_temp;
}

void mock_set_inverter_frequency(float freq) {
    INVERTER_FREQ_REG = freq;
}

void run_test_scenario(float initial_temp, float setpoint, const char* scenario_name, int* stable_time, float* overshoot) {
    printf("\n=== %s ===\n", scenario_name);
    printf("初始温度: %.1f°C, 设定温度: %.1f°C\n\n", initial_temp, setpoint);
    printf("时间(s)  设定温度  实际温度  误差    频率(Hz)\n");
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
        
        if (fabs(error) <= 0.2f) {
            if (!is_stable) {
                is_stable = 1;
                stable_start = t;
            }
        } else {
            is_stable = 0;
            stable_start = -1;
        }
        
        if (error * (setpoint - initial_temp) < 0) {
            float current_overshoot = fabs(error);
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
    
    printf("\n统计信息:\n");
    printf("稳定时间: %d秒\n", *stable_time);
    printf("最大超调量: %.2f°C\n", *overshoot);
    printf("最终误差: %.2f°C\n", setpoint - current_temp);
}

int main() {
    srand(time(NULL));
    
    printf("中央空调变频器温度闭环控制模拟\n");
    printf("================================\n");
    
    int stable_time1, stable_time2, stable_time3;
    float overshoot1, overshoot2, overshoot3;
    
    run_test_scenario(25.0f, 22.0f, "测试场景A: 降温过程(25℃→22℃)", &stable_time1, &overshoot1);
    
    run_test_scenario(18.0f, 22.0f, "测试场景B: 升温过程(18℃→22℃)", &stable_time2, &overshoot2);
    
    printf("\n=== 测试场景C: 温度波动场景 ===\n");
    printf("阶段1: 25℃→22℃, 阶段2: 22℃→24℃, 阶段3: 24℃→22℃\n\n");
    
    PIDController pid;
    pid_init(&pid, PID_KP, PID_KI, PID_KD, MIN_FREQ, MAX_FREQ);
    
    float current_temp = 25.0f;
    float setpoint = 22.0f;
    int phase = 1;
    int phase_start_time = 0;
    
    printf("时间(s)  设定温度  实际温度  误差    频率(Hz)  阶段\n");
    printf("---------------------------------------------------\n");
    
    for (int t = 0; t < SIMULATION_TIME; t++) {
        if (t == 100) {
            setpoint = 24.0f;
            phase = 2;
            phase_start_time = t;
            pid.integral = 0.0f;
        } else if (t == 200) {
            setpoint = 22.0f;
            phase = 3;
            phase_start_time = t;
            pid.integral = 0.0f;
        }
        
        float error = setpoint - current_temp;
        float output_freq = pid_update(&pid, setpoint, current_temp, SAMPLE_PERIOD);
        
        mock_set_inverter_frequency(output_freq);
        
        printf("%6d   %7.1f   %7.1f   %6.2f   %8.1f   %d\n", 
               t, setpoint, current_temp, error, output_freq, phase);
        
        current_temp = mock_read_temperature(setpoint, current_temp, output_freq, t);
    }
    
    printf("\n=== 测试总结 ===\n");
    printf("场景A(降温): 稳定时间=%d秒, 超调量=%.2f°C\n", stable_time1, overshoot1);
    printf("场景B(升温): 稳定时间=%d秒, 超调量=%.2f°C\n", stable_time2, overshoot2);
    printf("场景C(波动): 多阶段温度跟踪完成\n");
    
    return 0;
}