import random
import time

NUM_CHANNELS = 3
SAMPLES_PER_CHANNEL = 4
BUFFER_SIZE = NUM_CHANNELS * SAMPLES_PER_CHANNEL
SAMPLE_RATE_HZ = 1000

adc_buffer = [0] * BUFFER_SIZE
buffer_index = 0
sample_count = 0
stop_sampling = False

def adc_init():
    print("ADC initialized (3 channels: PA0, PA1, PA2)")

def dma_init():
    print(f"DMA initialized (circular buffer size: {BUFFER_SIZE})")

def adc_start():
    print("ADC started with DMA transfer")

def dma_irq_handler():
    global sample_count, stop_sampling
    sample_count += 1
    print(f"DMA Callback - Buffer ready (cycle {sample_count})")
    
    if sample_count >= 5:
        stop_sampling = True

def adc_simulate_conversion():
    global buffer_index
    for ch in range(NUM_CHANNELS):
        adc_buffer[buffer_index] = random.randint(0, 4095)
        buffer_index += 1
        
        if buffer_index >= BUFFER_SIZE:
            buffer_index = 0
            dma_irq_handler()

def print_channel_samples():
    print("\nFinal ADC Samples:")
    
    for ch in range(NUM_CHANNELS):
        samples = []
        for i in range(SAMPLES_PER_CHANNEL):
            idx = (BUFFER_SIZE - (NUM_CHANNELS - ch) - (i * NUM_CHANNELS))
            if idx < 0:
                idx += BUFFER_SIZE
            samples.append(adc_buffer[idx])
        
        print(f"Channel {ch+1} samples: {samples}")

def main():
    random.seed(time.time())
    
    print("STM32 ADC DMA Simulation Started")
    print("================================")
    
    adc_init()
    dma_init()
    adc_start()
    
    print("\nStarting data acquisition (5 cycles)...")
    
    while not stop_sampling:
        adc_simulate_conversion()
        time.sleep(0.001)  # 1ms delay
    
    print(f"\nData acquisition completed ({sample_count} cycles)")
    print_channel_samples()

if __name__ == "__main__":
    main()