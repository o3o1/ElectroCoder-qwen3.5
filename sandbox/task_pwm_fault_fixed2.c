#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int period_ms;
    unsigned int last_edge_time;
    int missing_count;
    int fault_flag;
} PWM_Monitor;

void pwm_monitor_init(PWM_Monitor *mon, int period_ms) {
    mon->period_ms = period_ms;
    mon->last_edge_time = 0;
    mon->missing_count = 0;
    mon->fault_flag = 0;
}

void pwm_edge_detected(PWM_Monitor *mon, unsigned int current_time) {
    mon->last_edge_time = current_time;
    mon->missing_count = 0;
}

int check_pwm_missing(PWM_Monitor *mon, unsigned int current_time) {
    if (mon->fault_flag) {
        return 1;
    }
    
    if (mon->last_edge_time == 0) {
        return 0;
    }
    
    unsigned int time_since_last_edge = current_time - mon->last_edge_time;
    
    if (time_since_last_edge > (mon->period_ms * 1.5)) {
        mon->missing_count++;
        
        if (mon->missing_count >= 3) {
            mon->fault_flag = 1;
        }
        
        return 1;
    }
    
    return 0;
}

int main() {
    PWM_Monitor monitor;
    pwm_monitor_init(&monitor, 20);
    
    int edge_times[] = {5, 25, 45, 65, 85};
    int edge_count = 5;
    int edge_index = 0;
    
    for (unsigned int time_ms = 0; time_ms <= 100; time_ms++) {
        if (edge_index < edge_count && time_ms == edge_times[edge_index]) {
            pwm_edge_detected(&monitor, time_ms);
            edge_index++;
        }
        
        int missing = check_pwm_missing(&monitor, time_ms);
        
        if (missing) {
            printf("Time %ums: Missing detected! Count=%d", time_ms, monitor.missing_count);
            if (monitor.fault_flag) {
                printf(" FAULT!");
            }
            printf("\n");
        }
    }
    
    return 0;
}