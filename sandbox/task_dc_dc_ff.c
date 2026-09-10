#include <stdio.h>
#include <math.h>

#define V_IN_NOM 12.0

double feedforward_compensation(double V_in, double V_ref, double V_out_meas, double K_ff) {
    double D = (V_ref / V_in) * (1.0 + K_ff * (V_IN_NOM - V_in) / V_IN_NOM);
    
    if (D < 0.05) D = 0.05;
    if (D > 0.95) D = 0.95;
    
    return D;
}

int main() {
    double V_ref = 5.0;
    double K_ff = 0.5;
    
    printf("DC/DC变换器前馈补偿算法测试\n");
    printf("V_ref=%.1fV, K_ff=%.1f, V_in_nom=%.1fV\n\n", V_ref, K_ff, V_IN_NOM);
    
    for (double V_in = 11.0; V_in <= 13.0; V_in += 0.5) {
        double V_out_meas = V_ref * (1.0 + 0.01 * (V_in - 12.0));
        double D = feedforward_compensation(V_in, V_ref, V_out_meas, K_ff);
        printf("V_in=%.1fV, D=%.3f\n", V_in, D);
    }
    
    return 0;
}