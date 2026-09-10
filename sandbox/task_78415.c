#include <stdio.h>

typedef struct {
    float V_batt;
    float V_ref;
    float delta_V;
    int mode;
} BidirectionalConverter;

void update_mode(BidirectionalConverter *conv) {
    float V_charge = conv->V_ref - conv->delta_V;
    float V_discharge = conv->V_ref + conv->delta_V;
    
    if (conv->mode == 0) {
        if (conv->V_batt > V_discharge) {
            conv->mode = 1;
        }
    } else {
        if (conv->V_batt < V_charge) {
            conv->mode = 0;
        }
    }
}

int main() {
    BidirectionalConverter conv;
    conv.V_ref = 48.0;
    conv.delta_V = 2.0;
    conv.mode = 0;
    
    float test_voltages[] = {46.5, 47.5, 49.5, 50.5, 49.0, 47.0, 46.0};
    int num_tests = sizeof(test_voltages) / sizeof(test_voltages[0]);
    
    for (int i = 0; i < num_tests; i++) {
        conv.V_batt = test_voltages[i];
        update_mode(&conv);
        
        const char* mode_str = (conv.mode == 0) ? "充电" : "放电";
        printf("V_batt=%.1fV, mode=%d(%s)\n", conv.V_batt, conv.mode, mode_str);
    }
    
    return 0;
}