#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// Mock hardware definitions
#define DC_BUS_VOLTAGE_MAX 800
#define PRECHARGE_VOLTAGE_THRESHOLD 600
#define PRECHARGE_TIMEOUT_MS 10000

// Mock control pins
#define PRECHARGE_CONTACTOR_PIN 0
#define MAIN_CONTACTOR_PIN 1
#define FAULT_SIGNAL_PIN 2

// State machine states
typedef enum {
    STATE_IDLE,
    STATE_PRECHARGE,
    STATE_RUN,
    STATE_FAULT
} PrechargeState;

// Global variables for simulation
static uint32_t mock_dc_voltage = 0;
static bool mock_fault_signal = false;
static uint32_t mock_gpio_output = 0;

// Mock functions for hardware interaction
static void mock_set_pin(uint8_t pin, bool state) {
    if (state) {
        mock_gpio_output |= (1 << pin);
    } else {
        mock_gpio_output &= ~(1 << pin);
    }
}

static uint32_t mock_read_dc_voltage(void) {
    return mock_dc_voltage;
}

static bool mock_check_fault(void) {
    return mock_fault_signal;
}

// Precharge controller structure
typedef struct {
    PrechargeState current_state;
    uint32_t precharge_start_time;
    bool start_command;
    bool reset_command;
    bool voltage_reached;
    uint32_t voltage_stable_time;
} PrechargeController;

// Initialize controller
void precharge_init(PrechargeController* ctrl) {
    ctrl->current_state = STATE_IDLE;
    ctrl->precharge_start_time = 0;
    ctrl->start_command = false;
    ctrl->reset_command = false;
    ctrl->voltage_reached = false;
    ctrl->voltage_stable_time = 0;
    
    // Initialize hardware to safe state
    mock_set_pin(PRECHARGE_CONTACTOR_PIN, false);
    mock_set_pin(MAIN_CONTACTOR_PIN, false);
}

// State machine execution
void precharge_run(PrechargeController* ctrl, uint32_t current_time) {
    // Check for fault signal in any state
    if (mock_check_fault()) {
        ctrl->current_state = STATE_FAULT;
        printf("Fault detected! Entering FAULT state\n");
    }
    
    switch (ctrl->current_state) {
        case STATE_IDLE:
            // Reset hardware
            mock_set_pin(PRECHARGE_CONTACTOR_PIN, false);
            mock_set_pin(MAIN_CONTACTOR_PIN, false);
            
            // Check for start command
            if (ctrl->start_command) {
                ctrl->current_state = STATE_PRECHARGE;
                ctrl->precharge_start_time = current_time;
                ctrl->start_command = false;
                printf("State transition: IDLE -> PRECHARGE\n");
            }
            break;
            
        case STATE_PRECHARGE:
            // Close precharge contactor, keep main contactor open
            mock_set_pin(PRECHARGE_CONTACTOR_PIN, true);
            mock_set_pin(MAIN_CONTACTOR_PIN, false);
            
            // Check for timeout
            if ((current_time - ctrl->precharge_start_time) >= PRECHARGE_TIMEOUT_MS) {
                ctrl->current_state = STATE_FAULT;
                printf("State transition: PRECHARGE -> FAULT (Timeout)\n");
            }
            // Check if voltage reached threshold
            else if (mock_read_dc_voltage() >= PRECHARGE_VOLTAGE_THRESHOLD) {
                // Additional check: voltage stable for 500ms
                if (!ctrl->voltage_reached) {
                    ctrl->voltage_reached = true;
                    ctrl->voltage_stable_time = current_time;
                    printf("Voltage reached threshold, starting stability timer...\n");
                }
                
                if ((current_time - ctrl->voltage_stable_time) >= 500) {
                    ctrl->current_state = STATE_RUN;
                    ctrl->voltage_reached = false;
                    float percentage = (mock_read_dc_voltage() * 100.0) / DC_BUS_VOLTAGE_MAX;
                    printf("State transition: PRECHARGE -> RUN (Voltage reached %.1f%%)\n", percentage);
                }
            } else {
                // Reset stable timer if voltage drops
                if (ctrl->voltage_reached) {
                    ctrl->voltage_reached = false;
                    printf("Voltage dropped below threshold, resetting stability timer\n");
                }
            }
            break;
            
        case STATE_RUN:
            // Close main contactor, open precharge contactor
            mock_set_pin(PRECHARGE_CONTACTOR_PIN, false);
            mock_set_pin(MAIN_CONTACTOR_PIN, true);
            break;
            
        case STATE_FAULT:
            // Open all contactors in fault state
            mock_set_pin(PRECHARGE_CONTACTOR_PIN, false);
            mock_set_pin(MAIN_CONTACTOR_PIN, false);
            
            // Check for reset command
            if (ctrl->reset_command) {
                ctrl->current_state = STATE_IDLE;
                ctrl->reset_command = false;
                printf("State transition: FAULT -> IDLE (Reset)\n");
            }
            break;
    }
}

// Helper function to print current state
const char* state_to_string(PrechargeState state) {
    switch (state) {
        case STATE_IDLE: return "IDLE";
        case STATE_PRECHARGE: return "PRECHARGE";
        case STATE_RUN: return "RUN";
        case STATE_FAULT: return "FAULT";
        default: return "UNKNOWN";
    }
}

// Test case 1: Normal precharge sequence
void test_normal_sequence(void) {
    printf("\n=== Test Case 1: Normal Precharge Sequence ===\n");
    
    PrechargeController ctrl;
    precharge_init(&ctrl);
    
    // Simulate normal startup
    ctrl.start_command = true;
    
    for (int i = 0; i < 20; i++) {
        // Simulate voltage rising
        mock_dc_voltage = 50 * i;
        uint32_t current_time = i * 100;
        
        precharge_run(&ctrl, current_time);
        printf("Time %dms: State=%s, Voltage=%dV, Precharge=%s, Main=%s\n", 
               i * 100, 
               state_to_string(ctrl.current_state), 
               mock_dc_voltage,
               (mock_gpio_output & (1 << PRECHARGE_CONTACTOR_PIN)) ? "ON" : "OFF",
               (mock_gpio_output & (1 << MAIN_CONTACTOR_PIN)) ? "ON" : "OFF");
    }
}

// Test case 2: Precharge timeout
void test_timeout_sequence(void) {
    printf("\n=== Test Case 2: Precharge Timeout ===\n");
    
    PrechargeController ctrl;
    precharge_init(&ctrl);
    
    // Simulate failed precharge (voltage stays low)
    ctrl.start_command = true;
    mock_dc_voltage = 100; // Low voltage that won't reach threshold
    
    for (int i = 0; i < 15; i++) {
        uint32_t current_time = i * 1000;
        precharge_run(&ctrl, current_time);
        printf("Time %dms: State=%s, Voltage=%dV, Precharge=%s, Main=%s\n", 
               i * 1000, 
               state_to_string(ctrl.current_state), 
               mock_dc_voltage,
               (mock_gpio_output & (1 << PRECHARGE_CONTACTOR_PIN)) ? "ON" : "OFF",
               (mock_gpio_output & (1 << MAIN_CONTACTOR_PIN)) ? "ON" : "OFF");
    }
}

// Test case 3: Fault during operation and reset
void test_fault_and_reset(void) {
    printf("\n=== Test Case 3: Fault and Reset ===\n");
    
    PrechargeController ctrl;
    precharge_init(&ctrl);
    
    // Start precharge
    ctrl.start_command = true;
    mock_dc_voltage = 300;
    
    for (int i = 0; i < 5; i++) {
        uint32_t current_time = i * 100;
        precharge_run(&ctrl, current_time);
        printf("Time %dms: State=%s, Voltage=%dV, Precharge=%s, Main=%s\n", 
               i * 100, 
               state_to_string(ctrl.current_state), 
               mock_dc_voltage,
               (mock_gpio_output & (1 << PRECHARGE_CONTACTOR_PIN)) ? "ON" : "OFF",
               (mock_gpio_output & (1 << MAIN_CONTACTOR_PIN)) ? "ON" : "OFF");
        
        if (i == 2) {
            // Inject fault
            mock_fault_signal = true;
            printf("*** Fault signal triggered ***\n");
        }
    }
    
    // Clear fault and reset
    mock_fault_signal = false;
    ctrl.reset_command = true;
    
    for (int i = 0; i < 3; i++) {
        uint32_t current_time = 500 + i * 100;
        precharge_run(&ctrl, current_time);
        printf("After reset %d: State=%s, Precharge=%s, Main=%s\n", 
               i, 
               state_to_string(ctrl.current_state),
               (mock_gpio_output & (1 << PRECHARGE_CONTACTOR_PIN)) ? "ON" : "OFF",
               (mock_gpio_output & (1 << MAIN_CONTACTOR_PIN)) ? "ON" : "OFF");
    }
}

int main(void) {
    printf("Inverter Precharge Control Simulation\n");
    printf("=====================================\n");
    
    // Run test cases
    test_normal_sequence();
    test_timeout_sequence();
    test_fault_and_reset();
    
    printf("\nAll test cases completed.\n");
    return 0;
}