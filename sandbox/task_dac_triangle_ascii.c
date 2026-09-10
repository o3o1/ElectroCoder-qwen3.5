#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// DAC hardware simulation
#define DAC_MAX_VALUE 4095  // 12-bit DAC maximum value
#define DAC_MIN_VALUE 0     // DAC minimum value

// Simulate DAC data register
uint16_t dac_output_value = 0;

// DAC set function
void DAC_SetValue(uint16_t value) {
    // Limit value to valid range
    if (value > DAC_MAX_VALUE) {
        dac_output_value = DAC_MAX_VALUE;
    } else if (value < DAC_MIN_VALUE) {
        dac_output_value = DAC_MIN_VALUE;
    } else {
        dac_output_value = value;
    }
}

// Triangle wave generation function
void generate_triangle_wave(uint16_t amplitude, uint16_t step_size, uint16_t cycles) {
    uint16_t current_value;
    uint16_t change_count = 0;
    bool increasing = true;
    
    // Limit amplitude to DAC maximum value
    if (amplitude > DAC_MAX_VALUE) {
        amplitude = DAC_MAX_VALUE;
    }
    
    for (uint16_t cycle = 0; cycle < cycles; cycle++) {
        // Start from minimum value
        current_value = DAC_MIN_VALUE;
        increasing = true;
        
        while (true) {
            // Set DAC output value
            DAC_SetValue(current_value);
            change_count++;
            
            // Print current value every 10 changes
            if (change_count % 10 == 0) {
                printf("  Change %d: DAC output = %u\n", change_count, dac_output_value);
            }
            
            // Determine direction and update value
            if (increasing) {
                // Increasing phase
                if (current_value + step_size > amplitude) {
                    current_value = amplitude;
                } else {
                    current_value += step_size;
                }
                
                // Check if amplitude is reached
                if (current_value >= amplitude) {
                    increasing = false;
                }
            } else {
                // Decreasing phase
                if (current_value < step_size) {
                    current_value = DAC_MIN_VALUE;
                } else {
                    current_value -= step_size;
                }
                
                // Check if back to minimum value
                if (current_value <= DAC_MIN_VALUE) {
                    break;  // Complete one cycle
                }
            }
        }
    }
    
    // Print total number of changes
    printf("  Total changes: %u\n", change_count);
}

int main() {
    printf("=== STM32 DAC Triangle Wave Generation Simulation ===\n\n");
    
    // Test case 1
    printf("Test 1: amplitude=2048, step=5, cycles=2\n");
    generate_triangle_wave(2048, 5, 2);
    printf("\n");
    
    // Test case 2
    printf("Test 2: amplitude=1000, step=2, cycles=1\n");
    generate_triangle_wave(1000, 2, 1);
    printf("\n");
    
    // Test case 3
    printf("Test 3: amplitude=5000(should be limited to 4095), step=8, cycles=1\n");
    generate_triangle_wave(5000, 8, 1);
    printf("\n");
    
    printf("=== All tests completed ===\n");
    return 0;
}