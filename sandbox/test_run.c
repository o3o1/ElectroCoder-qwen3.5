#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

// Mock hardware registers
#define ADC1_BASE 0x40012400
#define DMA1_BASE 0x40020000

#define ADC_CR2_ADON  (1 << 0)
#define ADC_CR2_CONT  (1 << 1)
#define ADC_CR2_DMA   (1 << 8)

#define DMA_CCR_EN    (1 << 0)
#define DMA_CCR_CIRC  (1 << 5)
#define DMA_CCR_HTIE  (1 << 2)
#define DMA_CCR_TCIE  (1 << 1)

// Configuration
#define NUM_CHANNELS  3
#define SAMPLES_PER_CHANNEL 4
#define BUFFER_SIZE   (NUM_CHANNELS * SAMPLES_PER_CHANNEL)  // 12
#define SAMPLE_RATE_HZ 1000

// Global variables
static uint16_t adc_buffer[BUFFER_SIZE];
static volatile int dma_half_transfer_flag = 0;
static volatile int dma_full_transfer_flag = 0;
static int sample_count = 0;
static int stop_sampling = 0;

// Mock ADC structure
typedef struct {
    volatile uint32_t SR;
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMPR1;
    volatile uint32_t SMPR2;
    volatile uint32_t JOFR[4];
    volatile uint32_t HTR;
    volatile uint32_t LTR;
    volatile uint32_t SQR1;
    volatile uint32_t SQR2;
    volatile uint32_t SQR3;
    volatile uint32_t JSQR;
    volatile uint32_t JDR[4];
    volatile uint32_t DR;
} ADC_TypeDef;

// Mock DMA structure
typedef struct {
    volatile uint32_t CCR;
    volatile uint32_t CNDTR;
    volatile uint32_t CPAR;
    volatile uint32_t CMAR;
    volatile uint32_t reserved;
} DMA_Channel_TypeDef;

// Mock DMA controller
typedef struct {
    volatile uint32_t ISR;
    volatile uint32_t IFCR;
    DMA_Channel_TypeDef CH[7];
} DMA_TypeDef;

// Mock peripheral pointers
ADC_TypeDef *ADC1 = (ADC_TypeDef *)ADC1_BASE;
DMA_TypeDef *DMA1 = (DMA_TypeDef *)DMA1_BASE;

// Function prototypes
void ADC_Init(void);
void DMA_Init(void);
void ADC_Start(void);
void DMA_IRQHandler(void);
void ADC_SimulateConversion(void);
void delay_ms(int ms);

// ADC initialization
void ADC_Init(void) {
    // Configure ADC for continuous conversion, DMA enabled
    ADC1->CR2 = ADC_CR2_CONT | ADC_CR2_DMA;
    
    // Configure sequence: 3 channels (CH1, CH2, CH3)
    ADC1->SQR1 = 0;  // 1 conversion in sequence
    ADC1->SQR3 = (0 << 0) | (1 << 5) | (2 << 10);  // CH0, CH1, CH2
    
    printf("ADC initialized (3 channels: PA0, PA1, PA2)\n");
}

// DMA initialization
void DMA_Init(void) {
    // Configure DMA channel for circular mode, half/full transfer interrupts
    DMA1->CH[1].CCR = DMA_CCR_CIRC | DMA_CCR_HTIE | DMA_CCR_TCIE;
    DMA1->CH[1].CNDTR = BUFFER_SIZE;
    DMA1->CH[1].CPAR = (uint32_t)&ADC1->DR;
    DMA1->CH[1].CMAR = (uint32_t)adc_buffer;
    
    printf("DMA initialized (circular buffer size: %d)\n", BUFFER_SIZE);
}

// Start ADC conversion
void ADC_Start(void) {
    ADC1->CR2 |= ADC_CR2_ADON;
    DMA1->CH[1].CCR |= DMA_CCR_EN;
    
    printf("ADC started with DMA transfer\n");
}

// DMA interrupt handler (simulated)
void DMA_IRQHandler(void) {
    // Check half transfer complete
    if (dma_half_transfer_flag) {
        printf("DMA Half Transfer Callback - First 6 samples ready\n");
        dma_half_transfer_flag = 0;
    }
    
    // Check transfer complete
    if (dma_full_transfer_flag) {
        printf("DMA Full Transfer Callback - All 12 samples ready\n");
        dma_full_transfer_flag = 0;
        sample_count++;
        
        // Stop after 5 cycles
        if (sample_count >= 5) {
            stop_sampling = 1;
        }
    }
}

// Simulate ADC conversion (generate random ADC values)
void ADC_SimulateConversion(void) {
    static int buffer_index = 0;
    
    // Generate random ADC values (0-4095 for 12-bit ADC)
    for (int ch = 0; ch < NUM_CHANNELS; ch++) {
        adc_buffer[buffer_index++] = rand() % 4096;
        
        // Wrap around circular buffer
        if (buffer_index >= BUFFER_SIZE) {
            buffer_index = 0;
            dma_full_transfer_flag = 1;
        }
        
        // Check half buffer
        if (buffer_index == BUFFER_SIZE / 2) {
            dma_half_transfer_flag = 1;
        }
    }
    
    // Simulate DMA interrupt
    if (dma_half_transfer_flag || dma_full_transfer_flag) {
        DMA_IRQHandler();
    }
}

// Simple delay function
void delay_ms(int ms) {
    // Simple busy wait for simulation
    clock_t start_time = clock();
    while ((clock() - start_time) < (ms * CLOCKS_PER_SEC / 1000)) {
        // Busy wait
    }
}

// Print channel samples
void print_channel_samples(void) {
    printf("\nFinal ADC Samples:\n");
    
    for (int ch = 0; ch < NUM_CHANNELS; ch++) {
        printf("Channel %d samples: [", ch + 1);
        
        // Print last 4 samples for this channel
        for (int i = 0; i < SAMPLES_PER_CHANNEL; i++) {
            // Calculate index: start from end of buffer and work backwards
            int idx = (BUFFER_SIZE - (NUM_CHANNELS - ch) - (i * NUM_CHANNELS));
            if (idx < 0) idx += BUFFER_SIZE;
            
            printf("%d", adc_buffer[idx]);
            if (i < SAMPLES_PER_CHANNEL - 1) {
                printf(", ");
            }
        }
        printf("]\n");
    }
}

int main(void) {
    // Initialize random seed
    srand(time(NULL));
    
    printf("STM32 ADC DMA Simulation Started\n");
    printf("================================\n");
    
    // Initialize peripherals
    ADC_Init();
    DMA_Init();
    
    // Start ADC conversion
    ADC_Start();
    
    printf("\nStarting data acquisition (5 cycles)...\n");
    
    // Main sampling loop
    while (!stop_sampling) {
        // Simulate ADC conversion
        ADC_SimulateConversion();
        
        // Simulate sampling interval (1ms for 1kHz)
        delay_ms(1);
    }
    
    printf("\nData acquisition completed (%d cycles)\n", sample_count);
    
    // Print final samples
    print_channel_samples();
    
    return 0;
}
