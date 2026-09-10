#include <stdio.h>
#include <math.h>

#define TS 0.001
#define SIM_TIME 1.0
#define STEPS 1000
#define PRINT_INTERVAL 100

typedef struct {
    double Kp;
    double Ki;
    double Kd;
    double integral;
    double prev_error;
    double prev_x_ref;
} PIDController;

double f(double x) {
    return -x*x + 1.0;
}

double g(double x) {
    return 2.0 + sin(x);
}

double pid_feedback_linearization_update(PIDController* ctrl, double x, double x_ref, double x_ref_derivative) {
    double error = x_ref - x;
    
    ctrl->integral += error * TS;
    
    double derivative = (error - ctrl->prev_error) / TS;
    
    double u = (1.0 / g(x)) * (-f(x) - ctrl->Kp * error - ctrl->Ki * ctrl->integral - ctrl->Kd * derivative + x_ref_derivative);
    
    ctrl->prev_error = error;
    ctrl->prev_x_ref = x_ref;
    
    return u;
}

void pid_controller_init(PIDController* ctrl, double Kp, double Ki, double Kd) {
    ctrl->Kp = Kp;
    ctrl->Ki = Ki;
    ctrl->Kd = Kd;
    ctrl->integral = 0.0;
    ctrl->prev_error = 0.0;
    ctrl->prev_x_ref = 0.0;
}

int main() {
    PIDController ctrl;
    pid_controller_init(&ctrl, 5.0, 0.5, 0.1);
    
    double x = 0.5;
    double x_ref = 1.0;
    double x_ref_derivative = 0.0;
    
    printf("Time(s)\t\tx\t\tu\t\terror\n");
    printf("--------------------------------------------\n");
    
    for (int i = 0; i <= STEPS; i++) {
        double t = i * TS;
        double u = pid_feedback_linearization_update(&ctrl, x, x_ref, x_ref_derivative);
        
        double dx = f(x) + g(x) * u;
        x += dx * TS;
        
        if (i % PRINT_INTERVAL == 0) {
            double error = x_ref - x;
            printf("%.6f\t%.6f\t%.6f\t%.6f\n", t, x, u, error);
        }
    }
    
    return 0;
}