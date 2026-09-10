#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

typedef struct {
    float target_power;
    float current_power;
    float pwm_duty;
    float kp;
    float ki;
    float integral_error;
} PowerControl;

void power_control_init(PowerControl *pc, float target, float kp, float ki) {
    pc->target_power = target;
    pc->current_power = 0.0f;
    pc->pwm_duty = 0.0f;
    pc->kp = kp;
    pc->ki = ki;
    pc->integral_error = 0.0f;
}

float read_power_sensor(void) {
    static float base_power = 0.0f;
    static int initialized = 0;
    
    if (!initialized) {
        srand(time(NULL));
        base_power = 400.0f + (rand() % 200);
        initialized = 1;
    }
    
    float noise = ((rand() % 100) - 50) / 10.0f;
    return base_power + noise;
}

void update_pwm(PowerControl *pc) {
    float error = pc->target_power - pc->current_power;
    pc->integral_error += error;
    
    float new_duty = pc->kp * error + pc->ki * pc->integral_error;
    
    if (new_duty < 0.0f) {
        pc->pwm_duty = 0.0f;
    } else if (new_duty > 1.0f) {
        pc->pwm_duty = 1.0f;
    } else {
        pc->pwm_duty = new_duty;
    }
}

void apply_pwm(float duty) {
    printf("Applied PWM duty: %.3f\n", duty);
}

void power_control_step(PowerControl *pc) {
    pc->current_power = read_power_sensor();
    update_pwm(pc);
    apply_pwm(pc->pwm_duty);
}

int main() {
    PowerControl pc;
    power_control_init(&pc, 500.0f, 0.01f, 0.001f);
    
    for (int i = 1; i <= 10; i++) {
        printf("Cycle %d: ", i);
        power_control_step(&pc);
        printf("current=%.1fW, target=%.1fW, duty=%.3f\n\n", 
               pc.current_power, pc.target_power, pc.pwm_duty);
    }
    
    return 0;
}