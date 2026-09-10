#include <stdio.h>
#include <math.h>

#define CAPACITANCE 0.001
#define VOLTAGE_THRESHOLD 0.1

typedef struct {
    double capacitor_voltage;
    int inserted;
    double energy;
} SubModule;

void calculate_energy(SubModule *sm) {
    sm->energy = 0.5 * CAPACITANCE * sm->capacitor_voltage * sm->capacitor_voltage;
}

void balance_control(SubModule sms[], int n, double target_voltage) {
    double sum_voltage = 0.0;
    int count_inserted = 0;
    
    for (int i = 0; i < n; i++) {
        if (sms[i].inserted == 1) {
            sum_voltage += sms[i].capacitor_voltage;
            count_inserted++;
        }
    }
    
    if (count_inserted == 0) return;
    
    double avg_voltage = sum_voltage / count_inserted;
    
    if (fabs(avg_voltage - target_voltage) > VOLTAGE_THRESHOLD) {
        int highest_idx = -1;
        double highest_voltage = -1.0;
        
        int lowest_idx = -1;
        double lowest_voltage = 1000.0;
        
        for (int i = 0; i < n; i++) {
            if (sms[i].inserted == 1 && sms[i].capacitor_voltage > highest_voltage) {
                highest_voltage = sms[i].capacitor_voltage;
                highest_idx = i;
            }
            
            if (sms[i].inserted == 0 && sms[i].capacitor_voltage < lowest_voltage) {
                lowest_voltage = sms[i].capacitor_voltage;
                lowest_idx = i;
            }
        }
        
        if (highest_idx != -1 && lowest_idx != -1) {
            sms[highest_idx].inserted = 0;
            sms[lowest_idx].inserted = 1;
            sms[lowest_idx].capacitor_voltage = target_voltage;
        }
    }
    
    for (int i = 0; i < n; i++) {
        calculate_energy(&sms[i]);
    }
}

void print_sm_status(SubModule sms[], int n, const char *title) {
    printf("\n%s:\n", title);
    printf("Index | Voltage (V) | Inserted | Energy (J)\n");
    printf("------|-------------|----------|------------\n");
    
    for (int i = 0; i < n; i++) {
        printf("%5d | %11.2f | %8d | %10.4f\n", 
               i, sms[i].capacitor_voltage, sms[i].inserted, sms[i].energy);
    }
}

int main() {
    SubModule sms[6];
    
    double initial_voltages[] = {99.8, 100.1, 100.0, 99.9, 100.2, 99.7};
    int initial_inserted[] = {1, 1, 1, 1, 0, 0};
    
    for (int i = 0; i < 6; i++) {
        sms[i].capacitor_voltage = initial_voltages[i];
        sms[i].inserted = initial_inserted[i];
        calculate_energy(&sms[i]);
    }
    
    print_sm_status(sms, 6, "Initial Status");
    
    balance_control(sms, 6, 100.0);
    
    print_sm_status(sms, 6, "After Balance Control");
    
    return 0;
}