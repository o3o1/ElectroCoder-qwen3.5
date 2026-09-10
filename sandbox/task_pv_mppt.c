#include <stdio.h>
#include <stdlib.h>

typedef struct {
    float V_pv;
    float I_pv;
    float P_pv;
} PVArray;

void update_pv_characteristics(PVArray* pv, float duty_cycle) {
    pv->V_pv = 40.0f - 15.0f * duty_cycle;
    pv->I_pv = 8.0f - 3.0f * duty_cycle;
    pv->P_pv = pv->V_pv * pv->I_pv;
}

float mppt_perturb_observe(PVArray* pv, float prev_power, float prev_duty) {
    float step = 0.01f;
    float new_duty = prev_duty;
    
    if (pv->P_pv > prev_power) {
        new_duty = prev_duty + step;
    } else {
        new_duty = prev_duty - step;
    }
    
    if (new_duty < 0.1f) new_duty = 0.1f;
    if (new_duty > 0.9f) new_duty = 0.9f;
    
    return new_duty;
}

int main() {
    PVArray pv;
    pv.V_pv = 30.0f;
    pv.I_pv = 5.0f;
    pv.P_pv = pv.V_pv * pv.I_pv;
    
    float duty_cycle = 0.5f;
    float prev_power = 0.0f;
    float max_power = 0.0f;
    float max_duty = 0.0f;
    float max_V = 0.0f;
    float max_I = 0.0f;
    
    printf("PV MPPT Simulation (Perturb and Observe Algorithm)\n");
    printf("==================================================\n");
    
    for (int i = 1; i <= 10; i++) {
        update_pv_characteristics(&pv, duty_cycle);
        pv.P_pv = pv.V_pv * pv.I_pv;
        
        if (i == 1) {
            prev_power = pv.P_pv;
        }
        
        printf("Iteration %d: V=%.2fV, I=%.2fA, P=%.2fW, D=%.2f\n", 
               i, pv.V_pv, pv.I_pv, pv.P_pv, duty_cycle);
        
        if (pv.P_pv > max_power) {
            max_power = pv.P_pv;
            max_duty = duty_cycle;
            max_V = pv.V_pv;
            max_I = pv.I_pv;
        }
        
        float new_duty = mppt_perturb_observe(&pv, prev_power, duty_cycle);
        prev_power = pv.P_pv;
        duty_cycle = new_duty;
    }
    
    printf("\nMax power point: V=%.2fV, I=%.2fA, P=%.2fW at D=%.2f\n",
           max_V, max_I, max_power, max_duty);
    
    return 0;
}