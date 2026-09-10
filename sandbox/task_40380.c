#include <stdio.h>

struct IGBT_Driver {
    float vcc_voltage;
    int output_state;
    float uvlo_threshold;
};

void check_undervoltage(struct IGBT_Driver *drv) {
    if (drv->vcc_voltage < drv->uvlo_threshold) {
        drv->output_state = 0;
        printf("UVLO triggered! VCC=%.2fV\n", drv->vcc_voltage);
    }
}

void set_vcc_voltage(struct IGBT_Driver *drv, float voltage) {
    drv->vcc_voltage = voltage;
    check_undervoltage(drv);
}

void init_driver(struct IGBT_Driver *drv, float init_voltage) {
    drv->vcc_voltage = init_voltage;
    drv->output_state = 1;
    drv->uvlo_threshold = 12.0f;
}

int main() {
    struct IGBT_Driver driver;
    
    init_driver(&driver, 15.0f);
    printf("Initial: VCC=%.2fV, output=%d\n", driver.vcc_voltage, driver.output_state);
    
    float test_voltages[] = {14.0f, 11.5f, 10.0f, 9.0f};
    
    for (int i = 0; i < 4; i++) {
        set_vcc_voltage(&driver, test_voltages[i]);
        printf("Set to %.2fV: output=%d\n", driver.vcc_voltage, driver.output_state);
    }
    
    return 0;
}