#include <stdio.h>
#include <stdlib.h>

typedef enum {
    PLL_LOCKED,
    PLL_UNLOCKED,
    PLL_RESTARTING
} PLL_Status;

typedef struct {
    PLL_Status current_state;
    float phase_error;
    int unlock_counter;
    int restart_step;
} PLL_State;

void pll_detect_unlock(PLL_State *pll, float phase_error) {
    if (phase_error > 0.5f) {
        pll->unlock_counter++;
        printf("UnlockCount=%d\n", pll->unlock_counter);
        if (pll->unlock_counter >= 3) {
            pll->current_state = PLL_UNLOCKED;
            printf("Enter UNLOCKED state!\n");
        }
    } else {
        pll->unlock_counter = 0;
    }
}

void pll_restart_sequence(PLL_State *pll) {
    switch (pll->restart_step) {
        case 0:
            printf("Step 1: Reset phase error\n");
            pll->phase_error = 0.0f;
            pll->restart_step = 1;
            break;
        case 1:
            printf("Step 2: Frequency pull-back\n");
            pll->restart_step = 2;
            break;
        case 2:
            printf("Step 3: Re-locking completed\n");
            pll->restart_step = 0;
            pll->current_state = PLL_LOCKED;
            pll->unlock_counter = 0;
            printf("Return to LOCKED state\n");
            break;
    }
}

void pll_update(PLL_State *pll, float new_phase_error) {
    pll->phase_error = new_phase_error;
    
    switch (pll->current_state) {
        case PLL_LOCKED:
            pll_detect_unlock(pll, new_phase_error);
            break;
        case PLL_UNLOCKED:
            pll->current_state = PLL_RESTARTING;
            pll->restart_step = 0;
            printf("Enter RESTARTING state...\n");
            break;
        case PLL_RESTARTING:
            pll_restart_sequence(pll);
            break;
    }
}

int main() {
    PLL_State pll = {
        .current_state = PLL_LOCKED,
        .phase_error = 0.0f,
        .unlock_counter = 0,
        .restart_step = 0
    };
    
    float phase_errors[] = {0.1f, 0.6f, 0.7f, 0.65f, 0.05f, 0.1f};
    int num_steps = sizeof(phase_errors) / sizeof(phase_errors[0]);
    
    for (int i = 0; i < num_steps; i++) {
        printf("Time %d: PhaseError=%.3f, State=", i+1, phase_errors[i]);
        
        switch (pll.current_state) {
            case PLL_LOCKED: printf("LOCKED"); break;
            case PLL_UNLOCKED: printf("UNLOCKED"); break;
            case PLL_RESTARTING: printf("RESTARTING"); break;
        }
        
        if (pll.current_state == PLL_LOCKED && pll.unlock_counter > 0) {
            printf(", UnlockCount=%d", pll.unlock_counter);
        } else if (pll.current_state == PLL_LOCKED) {
            printf(", Counter=0");
        }
        printf("\n");
        
        pll_update(&pll, phase_errors[i]);
    }
    
    return 0;
}