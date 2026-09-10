#include <stdio.h>
#include <stdint.h>

#define __IO volatile

// Register bit definitions
#define TIM_CR1_CEN_Pos  0
#define TIM_CR1_CEN_Msk  (1UL << TIM_CR1_CEN_Pos)

#define TIM_CR1_ARPE_Pos 7
#define TIM_CR1_ARPE_Msk (1UL << TIM_CR1_ARPE_Pos)

#define TIM_CCMR1_OC1M_Pos 4
#define TIM_CCMR1_OC1M_Msk (0x7UL << TIM_CCMR1_OC1M_Pos)
#define TIM_CCMR1_OC1PE_Pos 3
#define TIM_CCMR1_OC1PE_Msk (1UL << TIM_CCMR1_OC1PE_Pos)

#define TIM_CCMR1_OC2M_Pos 12
#define TIM_CCMR1_OC2M_Msk (0x7UL << TIM_CCMR1_OC2M_Pos)
#define TIM_CCMR1_OC2PE_Pos 11
#define TIM_CCMR1_OC2PE_Msk (1UL << TIM_CCMR1_OC2PE_Pos)

#define TIM_CCMR2_OC3M_Pos 4
#define TIM_CCMR2_OC3M_Msk (0x7UL << TIM_CCMR2_OC3M_Pos)
#define TIM_CCMR2_OC3PE_Pos 3
#define TIM_CCMR2_OC3PE_Msk (1UL << TIM_CCMR2_OC3PE_Pos)

#define TIM_CCMR2_OC4M_Pos 12
#define TIM_CCMR2_OC4M_Msk (0x7UL << TIM_CCMR2_OC4M_Pos)
#define TIM_CCMR2_OC4PE_Pos 11
#define TIM_CCMR2_OC4PE_Msk (1UL << TIM_CCMR2_OC4PE_Msk)

#define TIM_CCER_CC1E_Pos 0
#define TIM_CCER_CC1E_Msk (1UL << TIM_CCER_CC1E_Pos)
#define TIM_CCER_CC1P_Pos 1
#define TIM_CCER_CC1P_Msk (1UL << TIM_CCER_CC1P_Pos)

#define TIM_CCER_CC2E_Pos 4
#define TIM_CCER_CC2E_Msk (1UL << TIM_CCER_CC2E_Pos)
#define TIM_CCER_CC2P_Pos 5
#define TIM_CCER_CC2P_Msk (1UL << TIM_CCER_CC2P_Pos)

#define TIM_CCER_CC3E_Pos 8
#define TIM_CCER_CC3E_Msk (1UL << TIM_CCER_CC3E_Pos)
#define TIM_CCER_CC3P_Pos 9
#define TIM_CCER_CC3P_Msk (1UL << TIM_CCER_CC3P_Pos)

#define TIM_CCER_CC4E_Pos 12
#define TIM_CCER_CC4E_Msk (1UL << TIM_CCER_CC4E_Pos)
#define TIM_CCER_CC4P_Pos 13
#define TIM_CCER_CC4P_Msk (1UL << TIM_CCER_CC4P_Msk)

// PWM mode definitions
#define TIM_OCMODE_PWM1 6
#define TIM_OCMODE_PWM2 7

// Timer structure definition
typedef struct {
    __IO uint32_t CR1;      // Control register 1
    __IO uint32_t CR2;      // Control register 2
    __IO uint32_t SMCR;     // Slave mode control register
    __IO uint32_t DIER;     // DMA/interrupt enable register
    __IO uint32_t SR;       // Status register
    __IO uint32_t EGR;      // Event generation register
    __IO uint32_t CCMR1;    // Capture/compare mode register 1
    __IO uint32_t CCMR2;    // Capture/compare mode register 2
    __IO uint32_t CCER;     // Capture/compare enable register
    __IO uint32_t CNT;      // Counter
    __IO uint32_t PSC;      // Prescaler
    __IO uint32_t ARR;      // Auto-reload register
    __IO uint32_t CCR1;     // Capture/compare register 1
    __IO uint32_t CCR2;     // Capture/compare register 2
    __IO uint32_t CCR3;     // Capture/compare register 3
    __IO uint32_t CCR4;     // Capture/compare register 4
} TIM_TypeDef;

// Function declarations
void TIM_PWM_Init(TIM_TypeDef* TIMx, uint32_t prescaler, uint32_t period, uint32_t pulse);
void TIM_PWM_Start(TIM_TypeDef* TIMx, uint8_t channel);
void TIM_PWM_SetDutyCycle(TIM_TypeDef* TIMx, uint8_t channel, uint32_t pulse);
void print_registers(TIM_TypeDef* TIMx);

void TIM_PWM_Init(TIM_TypeDef* TIMx, uint32_t prescaler, uint32_t period, uint32_t pulse) {
    // 1. Configure prescaler and auto-reload value
    TIMx->PSC = prescaler;
    TIMx->ARR = period;
    
    // 2. Configure channel 1 as PWM mode 1
    // Clear OC1M bits
    TIMx->CCMR1 &= ~TIM_CCMR1_OC1M_Msk;
    // Set OC1M to PWM mode 1
    TIMx->CCMR1 |= (TIM_OCMODE_PWM1 << TIM_CCMR1_OC1M_Pos);
    // Enable preload register
    TIMx->CCMR1 |= TIM_CCMR1_OC1PE_Msk;
    
    // 3. Configure channel 2 as PWM mode 1
    TIMx->CCMR1 &= ~TIM_CCMR1_OC2M_Msk;
    TIMx->CCMR1 |= (TIM_OCMODE_PWM1 << TIM_CCMR1_OC2M_Pos);
    TIMx->CCMR1 |= TIM_CCMR1_OC2PE_Msk;
    
    // 4. Configure channel 3 as PWM mode 1
    TIMx->CCMR2 &= ~TIM_CCMR2_OC3M_Msk;
    TIMx->CCMR2 |= (TIM_OCMODE_PWM1 << TIM_CCMR2_OC3M_Pos);
    TIMx->CCMR2 |= TIM_CCMR2_OC3PE_Msk;
    
    // 5. Configure channel 4 as PWM mode 1
    TIMx->CCMR2 &= ~TIM_CCMR2_OC4M_Msk;
    TIMx->CCMR2 |= (TIM_OCMODE_PWM1 << TIM_CCMR2_OC4M_Pos);
    TIMx->CCMR2 |= TIM_CCMR2_OC4PE_Msk;
    
    // 6. Set initial pulse width
    TIMx->CCR1 = pulse;
    TIMx->CCR2 = pulse;
    TIMx->CCR3 = pulse;
    TIMx->CCR4 = pulse;
    
    // 7. Enable ARR preload
    TIMx->CR1 |= TIM_CR1_ARPE_Msk;
    
    // 8. Initialize counter
    TIMx->CNT = 0;
}

void TIM_PWM_Start(TIM_TypeDef* TIMx, uint8_t channel) {
    switch(channel) {
        case 1:
            // Enable channel 1 output
            TIMx->CCER |= TIM_CCER_CC1E_Msk;
            // Set output polarity to active high
            TIMx->CCER &= ~TIM_CCER_CC1P_Msk;
            break;
        case 2:
            TIMx->CCER |= TIM_CCER_CC2E_Msk;
            TIMx->CCER &= ~TIM_CCER_CC2P_Msk;
            break;
        case 3:
            TIMx->CCER |= TIM_CCER_CC3E_Msk;
            TIMx->CCER &= ~TIM_CCER_CC3P_Msk;
            break;
        case 4:
            TIMx->CCER |= TIM_CCER_CC4E_Msk;
            TIMx->CCER &= ~TIM_CCER_CC4P_Msk;
            break;
        default:
            return;
    }
    
    // Enable timer counter
    TIMx->CR1 |= TIM_CR1_CEN_Msk;
}

void TIM_PWM_SetDutyCycle(TIM_TypeDef* TIMx, uint8_t channel, uint32_t pulse) {
    if (pulse > TIMx->ARR) {
        pulse = TIMx->ARR;  // Limit pulse width not exceeding period
    }
    
    switch(channel) {
        case 1:
            TIMx->CCR1 = pulse;
            break;
        case 2:
            TIMx->CCR2 = pulse;
            break;
        case 3:
            TIMx->CCR3 = pulse;
            break;
        case 4:
            TIMx->CCR4 = pulse;
            break;
        default:
            return;
    }
}

void print_registers(TIM_TypeDef* TIMx) {
    printf("=== Timer Register Status ===\n");
    printf("CR1:   0x%08X\n", TIMx->CR1);
    printf("CR2:   0x%08X\n", TIMx->CR2);
    printf("CCMR1: 0x%08X\n", TIMx->CCMR1);
    printf("CCMR2: 0x%08X\n", TIMx->CCMR2);
    printf("CCER:  0x%08X\n", TIMx->CCER);
    printf("CNT:   0x%08X\n", TIMx->CNT);
    printf("PSC:   0x%08X\n", TIMx->PSC);
    printf("ARR:   0x%08X\n", TIMx->ARR);
    printf("CCR1:  0x%08X\n", TIMx->CCR1);
    printf("CCR2:  0x%08X\n", TIMx->CCR2);
    printf("CCR3:  0x%08X\n", TIMx->CCR3);
    printf("CCR4:  0x%08X\n", TIMx->CCR4);
    printf("============================\n");
}

int main() {
    // Declare timer structure variable
    TIM_TypeDef TIM1 = {0};
    
    printf("Initializing timer PWM configuration...\n");
    
    // Initialize timer PWM mode
    // Parameters: timer pointer, prescaler, period, initial pulse width
    TIM_PWM_Init(&TIM1, 7199, 9999, 5000);  // 72MHz/7200=10kHz, period 10000, duty cycle 50%
    
    printf("Starting channel 1 PWM output...\n");
    TIM_PWM_Start(&TIM1, 1);
    
    printf("Adjusting channel 1 duty cycle to 75%%...\n");
    TIM_PWM_SetDutyCycle(&TIM1, 1, 7500);
    
    // Print register status to verify configuration
    print_registers(&TIM1);
    
    // Calculate and display actual configuration
    printf("\nConfiguration parameters:\n");
    printf("Clock frequency: 72MHz\n");
    printf("Prescaler value: %u\n", TIM1.PSC + 1);
    printf("Auto-reload value: %u\n", TIM1.ARR);
    printf("Channel 1 pulse width: %u\n", TIM1.CCR1);
    printf("Channel 1 duty cycle: %.1f%%\n", (float)TIM1.CCR1 * 100.0f / (float)TIM1.ARR);
    
    return 0;
}
