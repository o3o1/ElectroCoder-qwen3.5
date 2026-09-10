#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

typedef struct {
    int id;
    double capacitor_voltage;
    int inserted;
} SubModule;

void initialize_sms(SubModule sms[], int n, double base_voltage) {
    for (int i = 0; i < n; i++) {
        sms[i].id = i;
        double random_factor = 0.9 + (rand() % 21) / 100.0;
        sms[i].capacitor_voltage = base_voltage * random_factor;
        sms[i].inserted = rand() % 2;
    }
}

void balance_control(SubModule sms[], int n, double target_voltage) {
    double sum_voltage = 0.0;
    int inserted_count = 0;
    
    for (int i = 0; i < n; i++) {
        if (sms[i].inserted == 1) {
            sum_voltage += sms[i].capacitor_voltage;
            inserted_count++;
        }
    }
    
    if (inserted_count == 0) return;
    
    double avg_voltage = sum_voltage / inserted_count;
    
    for (int i = 0; i < n; i++) {
        if (sms[i].inserted == 1) {
            if (sms[i].capacitor_voltage > avg_voltage * 1.05) {
                sms[i].inserted = 0;
                inserted_count--;
            }
        } else {
            if (sms[i].capacitor_voltage < avg_voltage * 0.95) {
                sms[i].inserted = 1;
                inserted_count++;
            }
        }
    }
    
    int min_inserted = n / 2;
    if (inserted_count < min_inserted) {
        int need_to_insert = min_inserted - inserted_count;
        for (int i = 0; i < n && need_to_insert > 0; i++) {
            if (sms[i].inserted == 0) {
                sms[i].inserted = 1;
                need_to_insert--;
            }
        }
    }
}

void print_status(SubModule sms[], int n) {
    for (int i = 0; i < n; i++) {
        printf("SM[%d]: voltage=%.2fV, inserted=%d\n", 
               sms[i].id, sms[i].capacitor_voltage, sms[i].inserted);
    }
    printf("\n");
}

int main() {
    srand(time(0));
    
    const int N = 8;
    SubModule sms[N];
    
    printf("=== Initial Status ===\n");
    initialize_sms(sms, N, 100.0);
    print_status(sms, N);
    
    for (int iter = 1; iter <= 5; iter++) {
        printf("=== Balance Control Iteration %d ===\n", iter);
        balance_control(sms, N, 100.0);
        print_status(sms, N);
    }
    
    return 0;
}