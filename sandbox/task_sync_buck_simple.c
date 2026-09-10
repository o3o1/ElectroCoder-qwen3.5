#include <stdio.h>

#define T_CYCLE 100
#define DUTY_CYCLE 0.6
#define DEAD_TIME 2
#define CURRENT_THRESHOLD 0.1

int main() {
    printf("Synchronous Rectifier Buck Converter Body Diode Suppression Simulation\n");
    printf("======================================================================\n\n");
    
    float test_currents[] = {0.5, 0.05, -0.2};
    char* test_names[] = {"Positive large current", "Positive small current", "Reverse current"};
    
    int high_on_time = (int)(T_CYCLE * DUTY_CYCLE);
    
    for (int i = 0; i < 3; i++) {
        float current = test_currents[i];
        printf("=== Test case: %s (I_L = %.3fA) ===\n", test_names[i], current);
        printf("Cycle timing: High-side ON=%dus, OFF=%dus, Dead time=%dus\n", 
               high_on_time, T_CYCLE - high_on_time, DEAD_TIME);
        
        for (int t = 0; t < T_CYCLE; t++) {
            if (t < high_on_time) {
                printf("t=%dus: High=1, Low=0\n", t);
            } else if (t < high_on_time + DEAD_TIME) {
                if (current > CURRENT_THRESHOLD) {
                    printf("t=%dus: High=0, Low=1 [Body diode suppression active, I_L=%.3fA>%.3fA]\n", 
                           t, current, CURRENT_THRESHOLD);
                } else {
                    printf("t=%dus: High=0, Low=0 [Body diode freewheeling, I_L=%.3fA<=%.3fA]\n", 
                           t, current, CURRENT_THRESHOLD);
                }
            } else {
                printf("t=%dus: High=0, Low=1\n", t);
            }
        }
        printf("\n");
    }
    
    printf("Simulation completed\n");
    return 0;
}