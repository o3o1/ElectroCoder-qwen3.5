#include <stdio.h>

#define R_S 0.5f

static float psi_alpha_global = 0.0f;
static float psi_beta_global = 0.0f;

void flux_observer(float u_alpha, float u_beta, float i_alpha, float i_beta, float *psi_alpha, float *psi_beta, float dt) {
    float u_alpha_comp = u_alpha - R_S * i_alpha;
    float u_beta_comp = u_beta - R_S * i_beta;
    
    psi_alpha_global = psi_alpha_global + u_alpha_comp * dt;
    psi_beta_global = psi_beta_global + u_beta_comp * dt;
    
    *psi_alpha = psi_alpha_global;
    *psi_beta = psi_beta_global;
}

int main() {
    float psi_alpha, psi_beta;
    float dt = 0.001f;
    
    psi_alpha_global = 0.0f;
    psi_beta_global = 0.0f;
    
    float test_inputs[3][4] = {
        {100.0f, 0.0f, 10.0f, 0.0f},
        {0.0f, 100.0f, 0.0f, 10.0f},
        {-100.0f, 0.0f, -10.0f, 0.0f}
    };
    
    for (int step = 0; step < 3; step++) {
        flux_observer(test_inputs[step][0], test_inputs[step][1], 
                      test_inputs[step][2], test_inputs[step][3],
                      &psi_alpha, &psi_beta, dt);
        
        printf("Step%d: psi_alpha=%.3f, psi_beta=%.3f\n", step+1, psi_alpha, psi_beta);
    }
    
    return 0;
}