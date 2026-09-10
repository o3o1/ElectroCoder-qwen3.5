#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int id;
    float voltage;
    float threshold;
    int status;
} Capacitor;

void check_balance(Capacitor caps[], int n) {
    float sum_voltage = 0.0f;
    int has_alert = 0;
    
    for (int i = 0; i < n; i++) {
        sum_voltage += caps[i].voltage;
        
        if (caps[i].voltage > caps[i].threshold) {
            caps[i].status = 1;
            has_alert = 1;
            printf("ALERT: Capacitor %d overvoltage! Voltage: %.2fV, Threshold: %.2fV\n", 
                   caps[i].id, caps[i].voltage, caps[i].threshold);
        } else {
            caps[i].status = 0;
        }
    }
    
    float avg_voltage = sum_voltage / n;
    printf("Average voltage: %.2fV\n", avg_voltage);
    
    if (!has_alert) {
        printf("All capacitors are normal.\n");
    }
}

int main() {
    Capacitor caps[5];
    
    caps[0].id = 1;
    caps[0].voltage = 95.5f;
    caps[0].threshold = 100.0f;
    caps[0].status = 0;
    
    caps[1].id = 2;
    caps[1].voltage = 105.2f;
    caps[1].threshold = 100.0f;
    caps[1].status = 0;
    
    caps[2].id = 3;
    caps[2].voltage = 98.7f;
    caps[2].threshold = 100.0f;
    caps[2].status = 0;
    
    caps[3].id = 4;
    caps[3].voltage = 110.8f;
    caps[3].threshold = 100.0f;
    caps[3].status = 0;
    
    caps[4].id = 5;
    caps[4].voltage = 97.3f;
    caps[4].threshold = 100.0f;
    caps[4].status = 0;
    
    check_balance(caps, 5);
    
    int normal_count = 0;
    for (int i = 0; i < 5; i++) {
        if (caps[i].status == 0) {
            normal_count++;
        }
    }
    
    printf("Normal capacitors: %d\n", normal_count);
    
    return 0;
}