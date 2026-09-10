#include <stdio.h>
#include <math.h>
#include <stdint.h>

// Mock hardware register definitions
#define ADC_CURRENT_REG (*((volatile uint16_t*)0x40012400))
#define GPIO_PROTECTION_PIN (*((volatile uint8_t*)0x40010800))
#define GPIO_SET 0x01
#define GPIO_RESET 0x00

// Overcurrent protection structure
typedef struct {
    double I_p;      // Protection threshold current
    double K;        // Time constant
    double alpha;    // Curve exponent
    double trip_time; // Calculated trip time
    int protection_triggered; // Protection status
} OvercurrentProtection;

// Function to calculate inverse time trip time
void calculate_inverse_time_trip(OvercurrentProtection *prot, double I_measured) {
    // Check if current is below threshold
    if (I_measured <= prot->I_p) {
        prot->trip_time = 0.0;
        prot->protection_triggered = 0;
        return;
    }
    
    // Calculate I/I_p ratio
    double ratio = I_measured / prot->I_p;
    
    // Calculate (I/I_p)^alpha
    double power_term = pow(ratio, prot->alpha);
    
    // Check denominator
    double denominator = power_term - 1.0;
    
    if (denominator <= 0.0) {
        // Immediate trip
        prot->trip_time = 0.0;
        prot->protection_triggered = 1;
    } else {
        // Calculate trip time: t = K / ((I/I_p)^alpha - 1)
        prot->trip_time = prot->K / denominator;
        prot->protection_triggered = 1;
    }
}

// Function to simulate protection action
void execute_protection_action(int triggered) {
    if (triggered) {
        GPIO_PROTECTION_PIN = GPIO_SET;
        printf("  Protection: TRIPPED (GPIO set)\n");
    } else {
        GPIO_PROTECTION_PIN = GPIO_RESET;
        printf("  Protection: NORMAL (GPIO reset)\n");
    }
}

// Function to run test case
void run_test_case(const char *name, double I_measured, double I_p, double K, double alpha) {
    printf("Test Case %s:\n", name);
    printf("  I_p = %.3f A, I = %.3f A (%.3f * I_p)\n", I_p, I_measured, I_measured/I_p);
    printf("  Parameters: K = %.3f, alpha = %.3f\n", K, alpha);
    
    OvercurrentProtection prot;
    prot.I_p = I_p;
    prot.K = K;
    prot.alpha = alpha;
    
    calculate_inverse_time_trip(&prot, I_measured);
    
    printf("  Trip time: %.3f seconds\n", prot.trip_time);
    execute_protection_action(prot.protection_triggered);
    printf("\n");
}

int main() {
    printf("=== Inverse Time Overcurrent Protection Simulation ===\n\n");
    
    // Base protection threshold
    double base_I_p = 100.0; // 100A
    
    // Test cases
    run_test_case("a", 1.2 * base_I_p, base_I_p, 10.0, 0.5);   // I=1.2*I_p, K=10, α=0.5
    run_test_case("b", 2.0 * base_I_p, base_I_p, 10.0, 1.0);   // I=2.0*I_p, K=10, α=1.0
    run_test_case("c", 1.05 * base_I_p, base_I_p, 10.0, 2.0);  // I=1.05*I_p, K=10, α=2.0
    run_test_case("d", 0.9 * base_I_p, base_I_p, 10.0, 1.0);   // I=0.9*I_p, K=10, α=1.0
    
    // Additional test case for immediate trip
    printf("Additional Test Case - Immediate Trip:\n");
    printf("  Testing boundary where (I/I_p)^alpha - 1 <= 0\n");
    run_test_case("e", 1.0 * base_I_p, base_I_p, 10.0, 0.5);   // I=I_p, should not trigger
    
    // Simulate ADC reading
    printf("Simulating ADC Reading:\n");
    uint16_t adc_raw_value = 2048; // Mock ADC value (12-bit ADC)
    double adc_scale_factor = 0.048828125; // Scale to current (100A full scale)
    double measured_current = adc_raw_value * adc_scale_factor;
    
    printf("  ADC raw value: %d\n", adc_raw_value);
    printf("  Measured current: %.3f A\n", measured_current);
    
    OvercurrentProtection real_prot;
    real_prot.I_p = 80.0; // 80A threshold
    real_prot.K = 15.0;   // Time constant
    real_prot.alpha = 1.5; // Curve exponent
    
    calculate_inverse_time_trip(&real_prot, measured_current);
    
    printf("  Protection threshold: %.3f A\n", real_prot.I_p);
    printf("  Trip time: %.3f seconds\n", real_prot.trip_time);
    execute_protection_action(real_prot.protection_triggered);
    
    return 0;
}