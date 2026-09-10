#include <stdio.h>
#include <math.h>

double feedforward_compensation(double v_out_measured, double i_load, double v_ref, double k_ff) {
    return v_ref + k_ff * (v_ref - v_out_measured) * fabs(i_load);
}

int main() {
    double test_data[3][3] = {
        {12.0, 2.5, 0.1},    // Vout=12.0V, Iload=2.5A, k_ff=0.1
        {11.8, 3.0, 0.15},   // Vout=11.8V, Iload=3.0A, k_ff=0.15
        {12.2, 1.8, 0.08}    // Vout=12.2V, Iload=1.8A, k_ff=0.08
    };
    
    double v_ref = 12.0;
    
    for (int i = 0; i < 3; i++) {
        double v_out = test_data[i][0];
        double i_load = test_data[i][1];
        double k_ff = test_data[i][2];
        
        double v_control = feedforward_compensation(v_out, i_load, v_ref, k_ff);
        
        printf("测试%d: Vout=%.2fV, Iload=%.2fA -> 控制电压=%.2fV\n", 
               i+1, v_out, i_load, v_control);
    }
    
    return 0;
}