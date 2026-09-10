#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#define NUM_CHANNELS 3
#define SAMPLES_PER_CHANNEL 4
#define BUFFER_SIZE (NUM_CHANNELS * SAMPLES_PER_CHANNEL)
#define SAMPLE_RATE_HZ 1000

static uint16_t adc_buffer[BUFFER_SIZE];
static int buffer_index = 0;
static int sample_count = 0;
static int stop_sampling = 0;

void ADC_Init(void) {
    printf("ADC initialized (3 channels: PA0, PA1, PA2)\n");
}

void DMA_Init(void) {
    printf("DMA initialized (circular buffer size: %d)\n", BUFFER_SIZE);
}

void ADC_Start(void) {
    printf("ADC started with DMA transfer\n");
}

void DMA_IRQHandler(void) {
    sample_count++;
    printf("DMA Callback - Buffer ready (cycle %d)\n", sample_count);
    
    if (sample_count >= 5) {
        stop_sampling = 1;
    }
}

void ADC_SimulateConversion(void) {
    for (int ch = 0; ch < NUM_CHANNELS; ch++) {
        adc_buffer[buffer_index++] = rand() % 4096;
        
        if (buffer_index >= BUFFER_SIZE) {
            buffer_index = 0;
            DMA_IRQHandler();
        }
    }
}

void delay_ms(int ms) {
    clock_t start_time = clock();
    while ((clock() - start_time) < (ms * CLOCKS_PER_SEC / 1000)) {
    }
}

void print_channel_samples(void) {
    printf("\nFinal ADC Samples:\n");
    
    for (int ch = 0; ch < NUM_CHANNELS; ch++) {
        printf("Channel %d samples: [", ch + 1);
        
        for (int i = 0; i < SAMPLES_PER_CHANNEL; i++) {
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
    srand(time(NULL));
    
    printf("STM32 ADC DMA Simulation Started\n");
    printf("================================\n");
    
    ADC_Init();
    DMA_Init();
    ADC_Start();
    
    printf("\nStarting data acquisition (5 cycles)...\n");
    
    while (!stop_sampling) {
        ADC_SimulateConversion();
        delay_ms(1);
    }
    
    printf("\nData acquisition completed (%d cycles)\n", sample_count);
    print_channel_samples();
    
    return 0;
}