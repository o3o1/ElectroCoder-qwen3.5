#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

// Watchdog structure
typedef struct {
    int counter;
    int timeout;
    bool restart_flag;
} Watchdog;

// Initialize watchdog
void watchdog_init(Watchdog *wd, int timeout) {
    wd->counter = 0;
    wd->timeout = timeout;
    wd->restart_flag = false;
}

// Feed watchdog
void feed_watchdog(Watchdog *wd) {
    wd->counter = 0;
}

// Update watchdog
void update_watchdog(Watchdog *wd) {
    wd->counter++;
    if (wd->counter >= wd->timeout) {
        wd->restart_flag = true;
    }
}

// Simulate main program loop
void simulate_main_program(Watchdog *wd, bool is_fault_test) {
    printf("--- Test Case %d: %s ---\n", 
           is_fault_test ? 2 : 1, 
           is_fault_test ? "Power Fault" : "Normal Feeding");
    
    srand(time(NULL) + (is_fault_test ? 100 : 0));
    
    int loop_count = 0;
    while (!wd->restart_flag) {
        // Simulate power fault: random > 0.9 means fault
        float random_val = (float)rand() / RAND_MAX;
        bool power_fault = (random_val > 0.9f);
        
        // If fault test and fault occurs, skip feeding
        if (is_fault_test && power_fault) {
            // Skip feeding during fault
            update_watchdog(wd);
            printf("Count:%d Status:Fault occurred, skip feeding\n", wd->counter);
        } else {
            // Normal case: feed watchdog
            feed_watchdog(wd);
            update_watchdog(wd);
            printf("Count:%d Status:Normal\n", wd->counter);
        }
        
        loop_count++;
        // Prevent infinite loop (for testing only)
        if (loop_count > 20) {
            printf("Too many loops, exit test\n");
            break;
        }
    }
    
    if (wd->restart_flag) {
        printf("Count:%d Status:Trigger restart!\n", wd->counter);
    }
}

int main() {
    // Test Case 1: Normal feeding
    Watchdog wd1;
    watchdog_init(&wd1, 5);
    simulate_main_program(&wd1, false);
    
    printf("\n");
    
    // Test Case 2: Power fault
    Watchdog wd2;
    watchdog_init(&wd2, 5);
    simulate_main_program(&wd2, true);
    
    return 0;
}