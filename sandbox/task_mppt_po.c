#include <stdio.h>
#include <math.h>

#define STEP_SIZE 0.5
#define VREF_MIN 10.0
#define VREF_MAX 50.0

double mppt_perturb_and_observe(double V, double I, double *Vref) {
    static double prev_power = 0.0;
    static int direction = 1;
    static int first_run = 1;
    
    double current_power = V * I;
    
    if (first_run) {
        first_run = 0;
        prev_power = current_power;
        return current_power;
    }
    
    if (current_power > prev_power) {
        direction = direction;
    } else {
        direction = -direction;
    }
    
    *Vref = *Vref + direction * STEP_SIZE;
    
    if (*Vref < VREF_MIN) {
        *Vref = VREF_MIN;
        direction = 1;
    }
    if (*Vref > VREF_MAX) {
        *Vref = VREF_MAX;
        direction = -1;
    }
    
    prev_power = current_power;
    return current_power;
}

double solar_panel_current(double V) {
    return 5.0 - 0.1 * V;
}

void test_mppt(double initial_vref) {
    printf("\n=== 测试开始，初始Vref=%.1fV ===\n", initial_vref);
    printf("迭代\tV(V)\tI(A)\tP(W)\tVref(V)\n");
    printf("----------------------------------------\n");
    
    double Vref = initial_vref;
    
    for (int i = 0; i < 10; i++) {
        double V = Vref;
        double I = solar_panel_current(V);
        double P = mppt_perturb_and_observe(V, I, &Vref);
        
        printf("%d\t%.2f\t%.2f\t%.2f\t%.2f\n", 
               i+1, V, I, P, Vref);
    }
}

int main() {
    printf("MPPT扰动观察法(P&O)算法测试\n");
    printf("太阳能电池板特性: I = 5.0 - 0.1 * V\n");
    printf("扰动步长: %.1fV, Vref范围: %.1fV ~ %.1fV\n\n", 
           STEP_SIZE, VREF_MIN, VREF_MAX);
    
    test_mppt(15.0);
    test_mppt(30.0);
    test_mppt(45.0);
    
    return 0;
}