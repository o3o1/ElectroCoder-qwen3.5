#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// 模拟硬件寄存器宏定义
#define GPIO_PIN_CS   0x01
#define GPIO_PIN_SCK  0x02
#define GPIO_PIN_MOSI 0x04

#define SPI_DR        0x00  // SPI数据寄存器
#define GPIO_ODR      0x00  // GPIO输出数据寄存器
#define GPIO_IDR      0x00  // GPIO输入数据寄存器

// 模拟GPIO操作
static uint8_t gpio_state = 0;

static void gpio_set(uint8_t pin) {
    gpio_state |= pin;
    printf("[GPIO] Set pin 0x%02X\n", pin);
}

static void gpio_reset(uint8_t pin) {
    gpio_state &= ~pin;
    printf("[GPIO] Reset pin 0x%02X\n", pin);
}

// SPI发送函数，模拟时钟下降沿发送数据
void spi_send(uint8_t data) {
    printf("[SPI] Sending byte: 0x%02X\n", data);
    
    // 模拟8个时钟周期
    for (int i = 7; i >= 0; i--) {
        // 时钟上升沿
        gpio_set(GPIO_PIN_SCK);
        printf("[SPI] SCK rising edge\n");
        
        // 设置MOSI数据位
        uint8_t bit = (data >> i) & 0x01;
        if (bit) {
            gpio_set(GPIO_PIN_MOSI);
        } else {
            gpio_reset(GPIO_PIN_MOSI);
        }
        printf("[SPI] MOSI = %d (bit %d)\n", bit, i);
        
        // 时钟下降沿（数据锁存）
        gpio_reset(GPIO_PIN_SCK);
        printf("[SPI] SCK falling edge - data latched\n");
    }
}

// DAC8552驱动函数
void dac8552_write(uint8_t channel, uint16_t value) {
    printf("\n[DAC8552] Writing to channel %c, value: 0x%04X\n", 
           (channel == 0) ? 'A' : 'B', value);
    
    // 构建24位数据帧
    uint32_t frame = 0;
    
    // 控制位构建
    // bit23=0 (保留)
    // bit22=0 (保留)
    // bit21=1 (缓冲使能)
    // bit20=1 (增益=1)
    // bit19=0 (关断禁用)
    // bit18~17: 通道选择 (00=通道A, 01=通道B)
    // bit16=0 (保留)
    
    uint8_t control = 0;
    control |= (1 << 5);  // bit21=1 (缓冲使能)
    control |= (1 << 4);  // bit20=1 (增益=1)
    
    // 通道选择
    if (channel == 0) {
        // 通道A: bit18~17 = 00
        control |= (0 << 2);  // bit18=0
        control |= (0 << 1);  // bit17=0
    } else {
        // 通道B: bit18~17 = 01
        control |= (0 << 2);  // bit18=0
        control |= (1 << 1);  // bit17=1
    }
    
    // 组合24位帧
    frame = ((uint32_t)control << 16) | value;
    
    printf("[DAC8552] 24-bit frame: 0x%06X\n", frame);
    printf("[DAC8552] Control byte: 0x%02X\n", control);
    
    // 片选低电平
    gpio_reset(GPIO_PIN_CS);
    printf("[DAC8552] CS low (开始传输)\n");
    
    // 发送24位数据（3个字节）
    spi_send((frame >> 16) & 0xFF);  // 高8位（控制位）
    spi_send((frame >> 8) & 0xFF);   // 中间8位
    spi_send(frame & 0xFF);          // 低8位
    
    // 片选高电平
    gpio_set(GPIO_PIN_CS);
    printf("[DAC8552] CS high (传输结束)\n");
}

int main() {
    printf("=== DAC8552 SPI Driver Simulation ===\n\n");
    
    // 初始化GPIO状态
    gpio_state = 0;
    gpio_set(GPIO_PIN_CS);  // 初始CS为高电平
    
    printf("\n--- Test Case a: 通道A输出满量程的50%% (0x8000) ---\n");
    dac8552_write(0, 0x8000);
    
    printf("\n--- Test Case b: 通道B输出满量程的25%% (0x4000) ---\n");
    dac8552_write(1, 0x4000);
    
    printf("\n--- Test Case c: 通道A输出最小值 (0x0000) ---\n");
    dac8552_write(0, 0x0000);
    
    printf("\n--- Test Case d: 通道B输出最大值 (0xFFFF) ---\n");
    dac8552_write(1, 0xFFFF);
    
    printf("\n=== Simulation Complete ===\n");
    return 0;
}