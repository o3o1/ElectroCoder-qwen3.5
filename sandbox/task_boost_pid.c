#include <stdio.h>

typedef struct {
    float Vin;
    float Vout_target;
    float Vout_current;
    float duty_cycle;
    float Kp;
    float Ki;
    float Kd;
    float integral_error;
    float last_error;
} BoostConverter;

void boost_init(BoostConverter *conv, float Vin, float Vout_target, float Kp, float Ki, float Kd) {
    conv->Vin = Vin;
    conv->Vout_target = Vout_target;
    conv->Vout_current = 0.0;
    conv->duty_cycle = 0.0;
    conv->Kp = Kp;
    conv->Ki = Ki;
    conv->Kd = Kd;
    conv->integral_error = 0.0;
    conv->last_error = 0.0;
}

float boost_pid_update(BoostConverter *conv, float Vout_measured) {
    float error = conv->Vout_target - Vout_measured;
    conv->integral_error += error;
    float derivative = error - conv->last_error;
    float pid_output = conv->Kp * error + conv->Ki * conv->integral_error + conv->Kd * derivative;
    conv->duty_cycle = 0.5 + pid_output * 0.1;
    
    if (conv->duty_cycle < 0.0) conv->duty_cycle = 0.0;
    if (conv->duty_cycle > 0.9) conv->duty_cycle = 0.9;
    
    conv->last_error = error;
    return conv->duty_cycle;
}

float boost_simulate_output(BoostConverter *conv, float load_current) {
    float Vout = conv->Vin / (1.0 - conv->duty_cycle) - load_current * 0.5;
    conv->Vout_current = Vout;
    return Vout;
}

int main() {
    BoostConverter converter;
    boost_init(&converter, 12.0, 24.0, 0.5, 0.01, 0.05);
    
    for (int i = 0; i < 5; i++) {
        float Vout = boost_simulate_output(&converter, 1.0);
        float duty = boost_pid_update(&converter, Vout);
        printf("Cycle %d: Vout=%.2fV, Duty=%.2f\n", i+1, Vout, duty);
    }
    
    return 0;
}