#include <stdio.h>
#include <stdlib.h>
#include <time.h>

float read_temperature(void) {
    static int initialized = 0;
    if (!initialized) {
        srand(time(NULL));
        initialized = 1;
    }
    return (float)(rand() % 101);
}

void set_pwm_duty_cycle(float duty) {
    printf("PWM duty cycle set to: %.1f%%\n", duty);
}

float temperature_control(float temp) {
    float duty = 0.0f;
    
    if (temp < 30.0f) {
        duty = 0.0f;
    } else if (temp >= 30.0f && temp <= 50.0f) {
        duty = (temp - 30.0f) * (50.0f / 20.0f);
    } else if (temp > 50.0f && temp <= 80.0f) {
        duty = 50.0f + (temp - 50.0f) * (50.0f / 30.0f);
    } else {
        duty = 100.0f;
    }
    
    if (duty < 0.0f) duty = 0.0f;
    if (duty > 100.0f) duty = 100.0f;
    
    return duty;
}

int main(void) {
    printf("Fan PWM Temperature Closed-loop Control System Simulation\n");
    printf("=========================================================\n\n");
    
    float test_temps[] = {25.0f, 40.0f, 65.0f, 90.0f};
    const char* test_cases[] = {"a) Temperature 25C", "b) Temperature 40C", "c) Temperature 65C", "d) Temperature 90C"};
    
    for (int i = 0; i < 4; i++) {
        printf("%s:\n", test_cases[i]);
        printf("  Current temperature: %.1fC\n", test_temps[i]);
        float duty = temperature_control(test_temps[i]);
        printf("  Calculated duty cycle: %.1f%%\n", duty);
        set_pwm_duty_cycle(duty);
        printf("\n");
    }
    
    printf("Random temperature test:\n");
    for (int i = 0; i < 5; i++) {
        float temp = read_temperature();
        float duty = temperature_control(temp);
        printf("  Temperature: %.1fC -> Duty cycle: %.1f%%\n", temp, duty);
    }
    
    return 0;
}