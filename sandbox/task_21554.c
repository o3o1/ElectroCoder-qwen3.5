#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define VOLTAGE_THRESHOLD_HIGH 650
#define VOLTAGE_THRESHOLD_LOW 600
#define VOLTAGE_MIN 300
#define VOLTAGE_MAX 800
#define PWM_STEP 10
#define NUM_CYCLES 10

int main() {
    srand(time(NULL));
    
    int current_pwm = 0;
    int brake_resistor_state = 0; // 0=OFF, 1=ON
    
    for (int cycle = 1; cycle <= NUM_CYCLES; cycle++) {
        int voltage = VOLTAGE_MIN + rand() % (VOLTAGE_MAX - VOLTAGE_MIN + 1);
        
        if (voltage > VOLTAGE_THRESHOLD_HIGH) {
            if (current_pwm < 100) {
                current_pwm += PWM_STEP;
                if (current_pwm > 100) current_pwm = 100;
            }
        } else if (voltage < VOLTAGE_THRESHOLD_LOW) {
            if (current_pwm > 0) {
                current_pwm -= PWM_STEP;
                if (current_pwm < 0) current_pwm = 0;
            }
        }
        
        brake_resistor_state = (current_pwm > 0) ? 1 : 0;
        
        printf("周期%d: 电压=%dV, PWM=%d%%, 状态=%s\n", 
               cycle, voltage, current_pwm, 
               brake_resistor_state ? "ON" : "OFF");
        
        sleep(0.5);
    }
    
    return 0;
}