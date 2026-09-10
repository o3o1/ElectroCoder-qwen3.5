#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define T_CYCLE 100
#define DUTY_CYCLE 0.6
#define DEAD_TIME 2
#define CURRENT_THRESHOLD 0.1

#define V_GS_HIGH 0
#define V_GS_LOW 1

int g_vgs_high = 0;
int g_vgs_low = 0;
float g_i_l = 0.0;

void simulate_cycle(float current) {
    int high_on_time = (int)(T_CYCLE * DUTY_CYCLE);
    int high_off_time = T_CYCLE - high_on_time;
    
    printf("Cycle timing: High-side ON=%dus, OFF=%dus, Dead time=%dus\n", 
           high_on_time, high_off_time, DEAD_TIME);
    
    for (int t = 0; t < T_CYCLE; t++) {
        if (t < high_on_time) {
            g_vgs_high = 1;
            g_vgs_low = 0;
        } else if (t < high_on_time + DEAD_TIME) {
            g_vgs_high = 0;
            if (current > CURRENT_THRESHOLD) {
                g_vgs_low = 1;
                printf("t=%dus: High=0, Low=1 [Body diode suppression active, I_L=%.3fA>%.3fA]\n", 
                       t, current, CURRENT_THRESHOLD);
            } else {
                g_vgs_low = 0;
                printf("t=%dus: High=0, Low=0 [Body diode freewheeling, I_L=%.3fA<=%.3fA]\n", 
                       t, current, CURRENT_THRESHOLD);
            }
        } else {
            g_vgs_high = 0;
            g_vgs_low = 1;
        }
    }
}

void test_case(float current, const char* case_name) {
    printf("\n=== Test case: %s (I_L = %.3fA) ===\n", case_name, current);
    simulate_cycle(current);
}

int main() {
    printf("Synchronous Rectifier Buck Converter Body Diode Suppression Simulation\n");
    printf("======================================================================\n");
    
    test_case(0.5, "Positive large current");
    test_case(0.05, "Positive small current");
    test_case(-0.2, "Reverse current");
    
    printf("\nSimulation completed\n");
    return 0;
}