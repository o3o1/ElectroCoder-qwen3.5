#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void vsg_pll_step(double grid_theta, double delta_t, double J, double D, double *theta, double *omega) {
    double delta_theta = grid_theta - *theta;
    double T_acc = delta_theta * D;
    *omega += (T_acc / J) * delta_t;
    *theta += *omega * delta_t;
    *theta = fmod(*theta + M_PI, 2 * M_PI) - M_PI;
}

int main() {
    double theta = 0.0;
    double omega = 314.16;
    double J = 0.1;
    double D = 10.0;
    double delta_t = 0.001;
    double t = 0.0;
    
    for (int i = 0; i < 200; i++) {
        double grid_theta;
        if (t < 0.1) {
            grid_theta = 314.16 * t;
        } else {
            grid_theta = 314.16 * t + 0.5;
        }
        
        vsg_pll_step(grid_theta, delta_t, J, D, &theta, &omega);
        
        if (i % 20 == 0) {
            printf("%.3f, %.4f, %.4f, %.4f\n", t, grid_theta, theta, omega);
        }
        
        t += delta_t;
    }
    
    return 0;
}