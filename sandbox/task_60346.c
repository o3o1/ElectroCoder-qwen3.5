#include <stdio.h>

struct IGBT_Driver {
    float vcc_voltage;
    int output_state;
    float uvlo_threshold;
};

void check_uvlo(struct IGBT_Driver *drv) {
    if (drv->vcc_voltage < drv->uvlo_threshold) {
        drv->output_state = 0;
        printf("UVLO triggered! Output forced OFF.\n");
    } else {
        printf("VCC normal. Output state maintained.\n");
    }
}

int main() {
    struct IGBT_Driver test1 = {15.0f, 1, 12.0f};
    struct IGBT_Driver test2 = {10.5f, 1, 12.0f};
    
    printf("Test case 1: VCC=%.1fV, threshold=%.1fV\n", test1.vcc_voltage, test1.uvlo_threshold);
    check_uvlo(&test1);
    printf("Final output state: %d\n\n", test1.output_state);
    
    printf("Test case 2: VCC=%.1fV, threshold=%.1fV\n", test2.vcc_voltage, test2.uvlo_threshold);
    check_uvlo(&test2);
    printf("Final output state: %d\n", test2.output_state);
    
    return 0;
}