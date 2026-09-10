#include <stdio.h>

int main() {
    float R_S = 0.5f;
    float psi_alpha = 0.0f;
    float psi_beta = 0.0f;
    float dt = 0.001f;
    
    float test_inputs[3][4] = {
        {100.0f, 0.0f, 10.0f, 0.0f},
        {0.0f, 100.0f, 0.0f, 10.0f},
        {-100.0f, 0.0f, -10.0f, 0.0f}
    };
    
    for (int step = 0; step < 3; step++) {
        float u_alpha = test_inputs[step][0];
        float u_beta = test_inputs[step][1];
        float i_alpha = test_inputs[step][2];
        float i_beta = test_inputs[step][3];
        
        float u_alpha_comp = u_alpha - R_S * i_alpha;
        float u_beta_comp = u_beta - R_S * i_beta;
        
        psi_alpha = psi_alpha + u_alpha_comp * dt;
        psi_beta = psi_beta + u_beta_comp * dt;
        
        printf("Step%d: psi_alpha=%.3f, psi_beta=%.3f\n", step+1, psi_alpha, psi_beta);
    }
    
    return 0;
}