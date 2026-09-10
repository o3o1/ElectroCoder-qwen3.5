#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_FAULTS 100
#define FAULT_FILE "fault_log.bin"

typedef struct {
    uint32_t fault_code;
    uint32_t timestamp;
    uint8_t fault_level;
    uint8_t is_read;
} FaultRecord;

typedef struct {
    FaultRecord records[MAX_FAULTS];
    uint32_t write_index;
    uint32_t read_index;
    uint32_t count;
    uint32_t unread_count;
} FaultLogSystem;

void fault_log_init(FaultLogSystem *sys) {
    memset(sys, 0, sizeof(FaultLogSystem));
    
    FILE *file = fopen(FAULT_FILE, "rb");
    if (file) {
        fread(sys, sizeof(FaultLogSystem), 1, file);
        fclose(file);
    }
}

void fault_log_save(FaultLogSystem *sys) {
    FILE *file = fopen(FAULT_FILE, "wb");
    if (file) {
        fwrite(sys, sizeof(FaultLogSystem), 1, file);
        fclose(file);
    }
}

void fault_log_add(FaultLogSystem *sys, uint32_t fault_code, uint8_t fault_level) {
    if (sys->count >= MAX_FAULTS) {
        sys->read_index = (sys->read_index + 1) % MAX_FAULTS;
        sys->count--;
        if (sys->records[sys->read_index].is_read == 0) {
            sys->unread_count--;
        }
    }
    
    FaultRecord *record = &sys->records[sys->write_index];
    record->fault_code = fault_code;
    record->timestamp = (uint32_t)time(NULL);
    record->fault_level = fault_level;
    record->is_read = 0;
    
    sys->write_index = (sys->write_index + 1) % MAX_FAULTS;
    sys->count++;
    sys->unread_count++;
    
    fault_log_save(sys);
}

int fault_log_read_earliest(FaultLogSystem *sys, FaultRecord *record) {
    if (sys->unread_count == 0) {
        return 0;
    }
    
    while (sys->records[sys->read_index].is_read) {
        sys->read_index = (sys->read_index + 1) % MAX_FAULTS;
    }
    
    *record = sys->records[sys->read_index];
    sys->records[sys->read_index].is_read = 1;
    sys->unread_count--;
    
    fault_log_save(sys);
    return 1;
}

uint32_t fault_log_get_count(FaultLogSystem *sys) {
    return sys->count;
}

uint32_t fault_log_get_unread_count(FaultLogSystem *sys) {
    return sys->unread_count;
}

void fault_log_clear(FaultLogSystem *sys) {
    memset(sys, 0, sizeof(FaultLogSystem));
    remove(FAULT_FILE);
}

void print_fault_record(FaultRecord *record) {
    printf("Fault Code: 0x%08X, Timestamp: %u, Level: %u, Status: %s\n",
           record->fault_code, record->timestamp, 
           record->fault_level, record->is_read ? "Read" : "Unread");
}

int main() {
    FaultLogSystem sys;
    FaultRecord record;
    
    printf("=== Test 1: Initialize System ===\n");
    fault_log_init(&sys);
    printf("Current fault count: %u, Unread count: %u\n\n", 
           fault_log_get_count(&sys), fault_log_get_unread_count(&sys));
    
    printf("=== Test 2: Add Multiple Fault Records ===\n");
    for (int i = 0; i < 105; i++) {
        fault_log_add(&sys, 0x1000 + i, i % 4);
        printf("Add fault %d: code=0x%04X, level=%d\n", 
               i + 1, 0x1000 + i, i % 4);
    }
    printf("Total faults: %u (should be <=100)\n", fault_log_get_count(&sys));
    printf("Unread faults: %u\n\n", fault_log_get_unread_count(&sys));
    
    printf("=== Test 3: Read Fault Records ===\n");
    uint32_t read_count = 0;
    while (fault_log_read_earliest(&sys, &record)) {
        printf("Read record %u: ", ++read_count);
        print_fault_record(&record);
    }
    printf("Total read: %u records\n", read_count);
    printf("Unread after reading: %u\n\n", fault_log_get_unread_count(&sys));
    
    printf("=== Test 4: Clear and Re-add Records ===\n");
    fault_log_clear(&sys);
    printf("Fault count after clear: %u\n", fault_log_get_count(&sys));
    
    fault_log_add(&sys, 0x2001, 2);
    fault_log_add(&sys, 0x2002, 1);
    fault_log_add(&sys, 0x2003, 3);
    
    printf("After adding 3 new records:\n");
    printf("Fault count: %u, Unread count: %u\n", 
           fault_log_get_count(&sys), fault_log_get_unread_count(&sys));
    
    printf("\n=== Test 5: Read Newly Added Records ===\n");
    while (fault_log_read_earliest(&sys, &record)) {
        print_fault_record(&record);
    }
    
    printf("\n=== Test Complete ===\n");
    return 0;
}