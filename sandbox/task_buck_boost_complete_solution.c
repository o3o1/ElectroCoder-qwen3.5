#include <stdio.h>
#include <math.h>

#define VIN 24.0
#define VREF -12.0
#define I_MAX 5.0
#define FSW 100000.0
#define T (1.0/FSW)
#define KP_V 0.05
#define KI_V 2.0
#define KP_I 0.3
#define RLOAD 6.0
#define L 100e-6
#define C 470e-6

typedef struct {
    double vout;
    double il;
    double integral_v;
} SystemState;

double voltage_pi_controller(double error, double dt, double *integral) {
    *integral += error * dt;
    return KP_V * error + KI_V * (*integral);
}

double current_p_controller(double error) {
    return KP_I * error;
}

void simulate_one_step(double duty, SystemState *state) {
    double dIL_dt = (VIN * duty - fabs(state->vout) * (1.0 - duty)) / L;
    double dVout_dt = (-state->il * (1.0 - duty) - state->vout / RLOAD) / C;
    
    state->il += dIL_dt * T;
    state->vout += dVout_dt * T;
}

int main() {
    SystemState state = {0.0, 0.0, 0.0};
    double duty = 0.333;  // 从理论占空比开始
    
    printf("周期序号\tVout(V)\t\tIL(A)\t\t占空比\n");
    printf("-------------------------------------------------\n");
    
    for (int i = 0; i < 1000; i++) {
        double v_error = VREF - state.vout;
        double i_ref = voltage_pi_controller(v_error, T, &state.integral_v);
        
        if (i_ref > I_MAX) i_ref = I_MAX;
        if (i_ref < -I_MAX) i_ref = -I_MAX;
        
        double i_error = i_ref - state.il;
        double duty_adjust = current_p_controller(i_error);
        duty += duty_adjust * T;  // 积分调整
        
        if (duty > 0.95) duty = 0.95;
        if (duty < 0.05) duty = 0.05;
        
        simulate_one_step(duty, &state);
        
        if (i % 100 == 0) {
            printf("%d\t\t%.3f\t\t%.3f\t\t%.3f\n", i, state.vout, state.il, duty);
        }
    }
    
    printf("\n最终状态：\n");
    printf("Vout = %.3f V (目标: %.1f V)\n", state.vout, VREF);
    printf("IL = %.3f A\n", state.il);
    printf("占空比 = %.3f\n", duty);
    
    double theoretical_duty = fabs(VREF) / (VIN + fabs(VREF));
    printf("理论占空比: %.3f\n", theoretical_duty);
    
    return 0;
}