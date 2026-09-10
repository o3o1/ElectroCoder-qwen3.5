#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define EEPROM_SIZE 1024
#define PARAM_AREA_SIZE 256
#define FAULT_AREA_SIZE 768
#define MAX_PARAMS 64
#define MAX_FAULTS 256
#define UNINITIALIZED_VALUE 0xFF

static uint8_t eeprom_memory[EEPROM_SIZE];

void eeprom_init(void) {
    memset(eeprom_memory, UNINITIALIZED_VALUE, EEPROM_SIZE);
}

int write_parameter(int index, float value) {
    if (index < 0 || index >= MAX_PARAMS) {
        return -1;
    }
    
    int addr = index * sizeof(float);
    if (addr + sizeof(float) > PARAM_AREA_SIZE) {
        return -1;
    }
    
    uint8_t *bytes = (uint8_t*)&value;
    for (int i = 0; i < sizeof(float); i++) {
        eeprom_memory[addr + i] = bytes[i];
    }
    
    return 0;
}

float read_parameter(int index) {
    if (index < 0 || index >= MAX_PARAMS) {
        return 0.0f;
    }
    
    int addr = index * sizeof(float);
    if (addr + sizeof(float) > PARAM_AREA_SIZE) {
        return 0.0f;
    }
    
    float value;
    uint8_t *bytes = (uint8_t*)&value;
    
    for (int i = 0; i < sizeof(float); i++) {
        bytes[i] = eeprom_memory[addr + i];
    }
    
    return value;
}

int log_fault(uint32_t timestamp, uint8_t code) {
    static int fault_index = 0;
    
    if (fault_index >= MAX_FAULTS) {
        fault_index = 0;
    }
    
    int base_addr = PARAM_AREA_SIZE + fault_index * (sizeof(uint32_t) + sizeof(uint8_t));
    
    if (base_addr + sizeof(uint32_t) + sizeof(uint8_t) > EEPROM_SIZE) {
        return -1;
    }
    
    uint8_t *timestamp_bytes = (uint8_t*)&timestamp;
    for (int i = 0; i < sizeof(uint32_t); i++) {
        eeprom_memory[base_addr + i] = timestamp_bytes[i];
    }
    
    eeprom_memory[base_addr + sizeof(uint32_t)] = code;
    
    int current_index = fault_index;
    fault_index++;
    
    return current_index;
}

int read_fault(int index, uint32_t *timestamp, uint8_t *code) {
    if (index < 0 || index >= MAX_FAULTS) {
        return -1;
    }
    
    int base_addr = PARAM_AREA_SIZE + index * (sizeof(uint32_t) + sizeof(uint8_t));
    
    if (base_addr + sizeof(uint32_t) + sizeof(uint8_t) > EEPROM_SIZE) {
        return -1;
    }
    
    uint8_t timestamp_bytes[sizeof(uint32_t)];
    for (int i = 0; i < sizeof(uint32_t); i++) {
        timestamp_bytes[i] = eeprom_memory[base_addr + i];
    }
    
    *timestamp = *(uint32_t*)timestamp_bytes;
    *code = eeprom_memory[base_addr + sizeof(uint32_t)];
    
    return 0;
}

int main() {
    printf("=== EEPROM Simulation Test ===\n");
    
    eeprom_init();
    printf("EEPROM initialization completed\n");
    
    printf("\n1. Parameter write test:\n");
    int ret;
    
    ret = write_parameter(0, 12.5f);
    printf("Write parameter[0]=12.5: %s\n", ret == 0 ? "Success" : "Failed");
    
    ret = write_parameter(1, 220.0f);
    printf("Write parameter[1]=220.0: %s\n", ret == 0 ? "Success" : "Failed");
    
    ret = write_parameter(2, 3.14f);
    printf("Write parameter[2]=3.14: %s\n", ret == 0 ? "Success" : "Failed");
    
    printf("\n2. Parameter read test:\n");
    printf("Read parameter[0]: %.2f\n", read_parameter(0));
    printf("Read parameter[1]: %.2f\n", read_parameter(1));
    printf("Read parameter[2]: %.2f\n", read_parameter(2));
    
    printf("\n3. Fault logging test:\n");
    int idx1 = log_fault(1000, 0x01);
    printf("Log fault 1: timestamp=1000, code=0x01, index=%d\n", idx1);
    
    int idx2 = log_fault(2000, 0x02);
    printf("Log fault 2: timestamp=2000, code=0x02, index=%d\n", idx2);
    
    int idx3 = log_fault(3000, 0x03);
    printf("Log fault 3: timestamp=3000, code=0x03, index=%d\n", idx3);
    
    printf("\n4. Fault reading test:\n");
    uint32_t ts;
    uint8_t code;
    
    for (int i = 0; i < 3; i++) {
        if (read_fault(i, &ts, &code) == 0) {
            printf("Fault record[%d]: timestamp=%u, code=0x%02X\n", i, ts, code);
        } else {
            printf("Read fault record[%d] failed\n", i);
        }
    }
    
    printf("\n5. Boundary test:\n");
    ret = write_parameter(-1, 99.9f);
    printf("Write parameter[-1]: %s\n", ret == -1 ? "Failed(correct)" : "Success(error)");
    
    ret = write_parameter(64, 99.9f);
    printf("Write parameter[64]: %s\n", ret == -1 ? "Failed(correct)" : "Success(error)");
    
    ret = write_parameter(63, 99.9f);
    printf("Write parameter[63]: %s\n", ret == 0 ? "Success" : "Failed");
    
    float val = read_parameter(-1);
    printf("Read parameter[-1]: %.2f (should be 0.0)\n", val);
    
    val = read_parameter(64);
    printf("Read parameter[64]: %.2f (should be 0.0)\n", val);
    
    printf("\n=== Test completed ===\n");
    
    return 0;
}