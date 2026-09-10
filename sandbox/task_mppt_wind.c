#include <stdio.h>
#include <math.h>

#define AIR_DENSITY 1.225
#define SWEEP_AREA 5026.55
#define CP_COEFFICIENT 0.48
#define RATED_POWER 1500.0
#define CUT_IN_SPEED 3.0
#define RATED_SPEED 12.0
#define CUT_OUT_SPEED 25.0

double calculate_mppt_power(double wind_speed) {
    if (wind_speed < CUT_IN_SPEED || wind_speed > CUT_OUT_SPEED) {
        return 0.0;
    }
    
    if (wind_speed >= RATED_SPEED) {
        return RATED_POWER;
    }
    
    double power = 0.5 * AIR_DENSITY * SWEEP_AREA * pow(wind_speed, 3) * CP_COEFFICIENT / 1000.0;
    return power;
}

int main() {
    double test_speeds[] = {2.5, 8.0, 15.0, 30.0};
    int num_tests = sizeof(test_speeds) / sizeof(test_speeds[0]);
    
    for (int i = 0; i < num_tests; i++) {
        double power = calculate_mppt_power(test_speeds[i]);
        printf("风速: %.1f m/s -> 功率: %.2f kW\n", test_speeds[i], power);
    }
    
    return 0;
}