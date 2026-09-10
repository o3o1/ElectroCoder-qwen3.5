#include <stdio.h>
#include <math.h>
#include <stdint.h>

// Global variables for numerical integration
static double last_delta_p = 0.0;
static double last_time = 0.0;
static int first_run = 1;
static double total_integral = 0.0;

double virtual_capacitor_control(double t, double P_ac, double f, double C_v, double k) {
    // Calculate grid angular frequency
    double omega = 2.0 * 3.14159265358979323846 * f;
    
    // Calculate instantaneous power ripple component Δp = P_ac * cos(2ωt)
    double delta_p = P_ac * cos(2.0 * omega * t);
    
    // Calculate integral term: v_c = (1/C_v) * ∫Δp dt
    double v_c = 0.0;
    
    if (first_run) {
        // First run, cannot perform numerical integration, return 0
        first_run = 0;
        last_delta_p = delta_p;
        last_time = t;
        total_integral = 0.0;
        v_c = 0.0;
    } else {
        // Use trapezoidal method for numerical integration
        double dt = t - last_time;
        if (dt > 0) {
            // Trapezoidal method: ∫f(t)dt ≈ (f(t1) + f(t2))/2 * dt
            double integral = (last_delta_p + delta_p) / 2.0 * dt;
            
            // Accumulate integral value
            total_integral += integral;
            
            // Calculate virtual capacitor voltage
            v_c = total_integral / C_v;
        }
        
        // Update previous values
        last_delta_p = delta_p;
        last_time = t;
    }
    
    // Calculate compensation voltage
    double v_comp = k * v_c;
    
    return v_comp;
}

// Reset function for multiple independent test cases
void reset_virtual_capacitor() {
    last_delta_p = 0.0;
    last_time = 0.0;
    first_run = 1;
    total_integral = 0.0;
}

int main() {
    printf("PV Grid Inverter Virtual Capacitor Power Decoupling Control Test\n");
    printf("===============================================================\n\n");
    
    // Test 1: Calculate compensation voltage at t=0.01s
    printf("Test 1: Single point calculation\n");
    printf("Parameters: f=50Hz, P_ac=100W, C_v=0.001F, k=0.5, t=0.01s\n");
    
    reset_virtual_capacitor();
    
    // Calculate an initial point first to ensure correct integration
    virtual_capacitor_control(0.0, 100.0, 50.0, 0.001, 0.5);
    
    // Calculate target point
    double v_comp1 = virtual_capacitor_control(0.01, 100.0, 50.0, 0.001, 0.5);
    printf("Compensation voltage v_comp = %.6f V\n\n", v_comp1);
    
    // Test 2: Calculate 5 consecutive points
    printf("Test 2: Calculate 5 consecutive points (t=0~0.02s, step=0.005s)\n");
    printf("Parameters: f=50Hz, P_ac=100W, C_v=0.001F, k=0.5\n");
    printf("Time(s)    Compensation Voltage(V)\n");
    printf("-----------------------------------\n");
    
    reset_virtual_capacitor();
    
    // Calculate 5 time points
    double time_points[] = {0.0, 0.005, 0.01, 0.015, 0.02};
    int num_points = sizeof(time_points) / sizeof(time_points[0]);
    
    for (int i = 0; i < num_points; i++) {
        double t = time_points[i];
        double v_comp = virtual_capacitor_control(t, 100.0, 50.0, 0.001, 0.5);
        printf("%.6f   %.6f\n", t, v_comp);
    }
    
    printf("\nTest completed!\n");
    
    return 0;
}