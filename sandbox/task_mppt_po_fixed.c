#include <stdio.h>
#include <math.h>

#define STEP 0.01f

static float prev_power = 0.0f;
static int first_run = 1;

float mppt_perturb_and_observe(float voltage, float current, float *duty_cycle) {
    float power = voltage * current;
    
    if (first_run) {
        first_run = 0;
        prev_power = power;
        return power;
    }
    
    float delta = power - prev_power;
    
    if (delta > 0.0f) {
        *duty_cycle += STEP;
    } else {
        *duty_cycle -= STEP;
    }
    
    if (*duty_cycle > 1.0f) *duty_cycle = 1.0f;
    if (*duty_cycle < 0.0f) *duty_cycle = 0.0f;
    
    prev_power = power;
    return power;
}

int main() {
    float duty_cycle = 0.5f;
    float voltage, current, power;
    
    printf("迭代次数\t占空比\t\t电压(V)\t\t电流(A)\t\t功率(W)\n");
    printf("----------------------------------------------------------------\n");
    
    for (int i = 0; i < 10; i++) {
        voltage = 20.0f * duty_cycle;
        current = 5.0f * (1.0f - 0.5f * fabs(duty_cycle - 0.75f));
        
        power = mppt_perturb_and_observe(voltage, current, &duty_cycle);
        
        printf("%d\t\t%.4f\t\t%.4f\t\t%.4f\t\t%.4f\n", 
               i+1, duty_cycle, voltage, current, power);
    }
    
    return 0;
}