#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define VOLTAGE_THRESHOLD_HIGH 650
#define VOLTAGE_THRESHOLD_LOW 600
#define VOLTAGE_MIN 300
#define VOLTAGE_MAX 800
#define PWM_STEP 10
#define NUM_CYCLES 10

int main() {
    srand((unsigned int)time(NULL));
    
    int current_pwm = 0;
    
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
        
        printf("周期%d: 电压=%dV, PWM=%d%%, 状态=%s\n", 
               cycle, voltage, current_pwm, 
               current_pwm > 0 ? "ON" : "OFF");
        
        #ifdef _WIN32
        Sleep(500);
        #else
        usleep(500000);
        #endif
    }
    
    return 0;
}