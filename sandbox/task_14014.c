#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

bool emergency_stop_handler(uint8_t estop_signals, uint8_t safety_loop_status) {
    // Condition a: any emergency stop signal is triggered (bit = 1)
    if (estop_signals != 0) {
        return true;
    }
    
    // Condition b: safety loop has open circuit (bit = 0)
    // Safety loop normal when bit = 1, open circuit when bit = 0
    if (safety_loop_status != 0xFF) {
        return true;
    }
    
    return false;
}

int main() {
    // Test 1: no emergency stop signals, safety loop all normal (should return false)
    uint8_t estop1 = 0x00;      // 00000000
    uint8_t safety1 = 0xFF;     // 11111111
    bool result1 = emergency_stop_handler(estop1, safety1);
    printf("Test 1 - No emergency stop signals, safety loop all normal: %s\n", result1 ? "true(stop)" : "false(normal)");
    
    // Test 2: bit 3 emergency stop signal triggered, safety loop all normal (should return true)
    uint8_t estop2 = 0x08;      // 00001000 (bit 3 = 1)
    uint8_t safety2 = 0xFF;     // 11111111
    bool result2 = emergency_stop_handler(estop2, safety2);
    printf("Test 2 - Bit 3 emergency stop signal triggered, safety loop all normal: %s\n", result2 ? "true(stop)" : "false(normal)");
    
    // Test 3: no emergency stop signals, safety loop bit 5 open circuit (should return true)
    uint8_t estop3 = 0x00;      // 00000000
    uint8_t safety3 = 0xDF;     // 11011111 (bit 5 = 0)
    bool result3 = emergency_stop_handler(estop3, safety3);
    printf("Test 3 - No emergency stop signals, safety loop bit 5 open circuit: %s\n", result3 ? "true(stop)" : "false(normal)");
    
    // Test 4: bit 0 and bit 7 emergency stop signals triggered, safety loop bit 2 open circuit (should return true)
    uint8_t estop4 = 0x81;      // 10000001 (bit 0 and bit 7 = 1)
    uint8_t safety4 = 0xFB;     // 11111011 (bit 2 = 0)
    bool result4 = emergency_stop_handler(estop4, safety4);
    printf("Test 4 - Bit 0 and bit 7 emergency stop signals triggered, safety loop bit 2 open circuit: %s\n", result4 ? "true(stop)" : "false(normal)");
    
    return 0;
}