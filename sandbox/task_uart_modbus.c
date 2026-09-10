#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define HOLDING_REGISTERS_SIZE 100
uint16_t holding_registers[HOLDING_REGISTERS_SIZE] = {0};

uint16_t crc16_modbus(uint8_t* data, uint16_t length) {
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc = crc >> 1;
            }
        }
    }
    return crc;
}

uint8_t* modbus_slave_process(uint8_t* request, uint16_t req_len, uint16_t* resp_len) {
    if (req_len < 6) return NULL;
    
    uint16_t crc_received = (request[req_len-1] << 8) | request[req_len-2];
    uint16_t crc_calculated = crc16_modbus(request, req_len-2);
    
    if (crc_received != crc_calculated) return NULL;
    
    uint8_t slave_addr = request[0];
    uint8_t function_code = request[1];
    
    uint8_t* response = NULL;
    
    if (function_code == 0x03) {
        uint16_t start_addr = (request[2] << 8) | request[3];
        uint16_t reg_count = (request[4] << 8) | request[5];
        
        if (start_addr + reg_count > HOLDING_REGISTERS_SIZE) return NULL;
        
        uint8_t byte_count = reg_count * 2;
        *resp_len = 5 + byte_count + 2;
        response = (uint8_t*)malloc(*resp_len);
        
        response[0] = slave_addr;
        response[1] = 0x03;
        response[2] = byte_count;
        
        for (uint16_t i = 0; i < reg_count; i++) {
            uint16_t reg_value = holding_registers[start_addr + i];
            response[3 + i*2] = (reg_value >> 8) & 0xFF;
            response[4 + i*2] = reg_value & 0xFF;
        }
        
        uint16_t crc = crc16_modbus(response, *resp_len - 2);
        response[*resp_len - 2] = crc & 0xFF;
        response[*resp_len - 1] = (crc >> 8) & 0xFF;
        
    } else if (function_code == 0x06) {
        uint16_t reg_addr = (request[2] << 8) | request[3];
        uint16_t reg_value = (request[4] << 8) | request[5];
        
        if (reg_addr >= HOLDING_REGISTERS_SIZE) return NULL;
        
        holding_registers[reg_addr] = reg_value;
        
        *resp_len = req_len;
        response = (uint8_t*)malloc(*resp_len);
        memcpy(response, request, *resp_len);
        
    } else {
        return NULL;
    }
    
    return response;
}

int main() {
    printf("Modbus RTU Slave Test\n");
    printf("=====================\n\n");
    
    uint16_t resp_len;
    
    printf("Test 1: Function Code 0x06 (Write Single Register)\n");
    printf("--------------------------------------------------\n");
    uint8_t write_request[] = {
        0x01, 0x06, 0x00, 0x02, 0x12, 0x34, 0x00, 0x00
    };
    uint16_t write_crc = crc16_modbus(write_request, 6);
    write_request[6] = write_crc & 0xFF;
    write_request[7] = (write_crc >> 8) & 0xFF;
    
    uint8_t* write_response = modbus_slave_process(write_request, 8, &resp_len);
    if (write_response) {
        printf("Write Response (%d bytes): ", resp_len);
        for (uint16_t i = 0; i < resp_len; i++) {
            printf("%02X ", write_response[i]);
        }
        printf("\n");
        free(write_response);
    } else {
        printf("Write request failed!\n");
    }
    
    printf("\nTest 2: Function Code 0x03 (Read Holding Registers)\n");
    printf("---------------------------------------------------\n");
    uint8_t read_request[] = {
        0x01, 0x03, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00
    };
    uint16_t read_crc = crc16_modbus(read_request, 6);
    read_request[6] = read_crc & 0xFF;
    read_request[7] = (read_crc >> 8) & 0xFF;
    
    uint8_t* read_response = modbus_slave_process(read_request, 8, &resp_len);
    if (read_response) {
        printf("Read Response (%d bytes): ", resp_len);
        for (uint16_t i = 0; i < resp_len; i++) {
            printf("%02X ", read_response[i]);
        }
        printf("\n");
        free(read_response);
    } else {
        printf("Read request failed!\n");
    }
    
    printf("\nTest 3: Verify Register Values\n");
    printf("------------------------------\n");
    printf("holding_registers[2] = 0x%04X (%d)\n", holding_registers[2], holding_registers[2]);
    printf("holding_registers[3] = 0x%04X (%d)\n", holding_registers[3], holding_registers[3]);
    
    return 0;
}