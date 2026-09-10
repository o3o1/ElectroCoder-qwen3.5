#include <stdio.h>
#include <string.h>

void generate_pulse_sequence(int duty_cycle, char *buffer) {
    int i;
    for (i = 0; i < 100; i++) {
        if (i < duty_cycle) {
            buffer[i] = 'H';
        } else {
            buffer[i] = 'L';
        }
    }
    buffer[100] = '\0';
}

void simulate_power_output(const char *sequence) {
    int i;
    for (i = 0; i < 5; i++) {
        if (sequence[i] == 'H') {
            printf("[POWER ON]\n");
        } else {
            printf("[POWER OFF]\n");
        }
    }
    
    printf("...\n");
    
    for (i = 95; i < 100; i++) {
        if (sequence[i] == 'H') {
            printf("[POWER ON]\n");
        } else {
            printf("[POWER OFF]\n");
        }
    }
}

int main() {
    int test_cases[] = {0, 25, 50, 75, 100};
    int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);
    char buffer[101];
    int i, j;
    
    for (i = 0; i < num_cases; i++) {
        printf("=== Duty Cycle: %d%% ===\n", test_cases[i]);
        generate_pulse_sequence(test_cases[i], buffer);
        simulate_power_output(buffer);
        printf("\n");
    }
    
    return 0;
}