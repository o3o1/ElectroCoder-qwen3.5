#include <stdio.h>

float droop_control(float P_measured, float P_nominal, float k_droop, float f_nominal) {
    return f_nominal - k_droop * (P_measured - P_nominal);
}

int main() {
    float P_meas_A = 8.0f;
    float P_nom_A = 10.0f;
    float k_A = 0.05f;
    float f_nom = 50.0f;
    
    float P_meas_B = 12.0f;
    float P_nom_B = 15.0f;
    float k_B = 0.04f;
    
    float f_A = droop_control(P_meas_A, P_nom_A, k_A, f_nom);
    float f_B = droop_control(P_meas_B, P_nom_B, k_B, f_nom);
    
    printf("Inverter A: P_meas=%.2fkW, P_nom=%.2fkW, k=%.2fHz/kW, f_nom=%.2fHz -> f=%.2fHz\n",
           P_meas_A, P_nom_A, k_A, f_nom, f_A);
    printf("Inverter B: P_meas=%.2fkW, P_nom=%.2fkW, k=%.2fHz/kW, f_nom=%.2fHz -> f=%.2fHz\n",
           P_meas_B, P_nom_B, k_B, f_nom, f_B);
    
    return 0;
}