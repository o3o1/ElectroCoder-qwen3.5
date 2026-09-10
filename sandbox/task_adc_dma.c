#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

// 模拟硬件寄存器宏定义
#define ADC_DR          (*((volatile uint16_t*)0x4001244C))
#define DMA_SxNDTR      (*((volatile uint32_t*)0x4002600C))
#define DMA_SxCR        (*((volatile uint32_t*)0x40026008))
#define ADC_CR2         (*((volatile uint32_t*)0x40012408))
#define ADC_SR          (*((volatile uint32_t*)0x40012400))

// 寄存器位定义
#define ADC_CR2_ADON    (1 << 0)
#define ADC_CR2_CONT    (1 << 1)
#define ADC_CR2_DMA     (1 << 8)
#define ADC_SR_EOC      (1 << 1)

#define DMA_SxCR_EN     (1 << 0)
#define DMA_SxCR_CIRC   (1 << 5)
#define DMA_SxCR_TCIE   (1 << 4)

// 配置参数
#define ADC_CHANNELS    3
#define SAMPLES_PER_CH  10
#define TOTAL_SAMPLES   (ADC_CHANNELS * SAMPLES_PER_CH)

// 全局变量
uint16_t dma_buffer[TOTAL_SAMPLES];
volatile uint32_t dma_transfer_complete = 0;
volatile uint32_t adc_conversion_count = 0;
volatile uint32_t current_channel = 0;

// 模拟ADC初始化
void adc_init(void) {
    ADC_CR2 = 0;
    ADC_SR = 0;
    
    // 使能ADC，连续转换模式，使能DMA
    ADC_CR2 = ADC_CR2_ADON | ADC_CR2_CONT | ADC_CR2_DMA;
    
    printf("ADC初始化完成\n");
}

// 模拟DMA初始化
void dma_init(void) {
    DMA_SxCR = 0;
    DMA_SxNDTR = 0;
    
    // 配置DMA：循环模式，使能传输完成中断
    DMA_SxCR = DMA_SxCR_CIRC | DMA_SxCR_TCIE;
    
    // 设置传输数据数量
    DMA_SxNDTR = TOTAL_SAMPLES;
    
    printf("DMA初始化完成，传输数据量：%d\n", TOTAL_SAMPLES);
}

// 模拟ADC转换完成中断
void simulate_adc_conversion_complete(void) {
    // 设置EOC标志
    ADC_SR |= ADC_SR_EOC;
    
    // 生成模拟ADC值（不同通道不同范围）
    uint16_t adc_value;
    switch(current_channel) {
        case 0:  // CH0: 1000-1100
            adc_value = 1000 + (rand() % 101);
            break;
        case 1:  // CH1: 2000-2100
            adc_value = 2000 + (rand() % 101);
            break;
        case 2:  // CH2: 3000-3100
            adc_value = 3000 + (rand() % 101);
            break;
        default:
            adc_value = 0;
    }
    
    // 写入ADC数据寄存器
    ADC_DR = adc_value;
    
    // 触发DMA传输
    if (DMA_SxCR & DMA_SxCR_EN) {
        uint32_t remaining = DMA_SxNDTR;
        if (remaining > 0) {
            // DMA自动传输数据到缓冲区
            dma_buffer[TOTAL_SAMPLES - remaining] = ADC_DR;
            DMA_SxNDTR = remaining - 1;
            
            // 更新通道索引
            current_channel = (current_channel + 1) % ADC_CHANNELS;
            adc_conversion_count++;
            
            // 检查是否传输完成
            if (remaining == 1) {
                dma_transfer_complete = 1;
            }
        }
    }
    
    // 清除EOC标志
    ADC_SR &= ~ADC_SR_EOC;
}

// DMA传输完成回调
void dma_transfer_complete_callback(void) {
    printf("\nDMA传输完成！\n");
    printf("总转换次数：%d\n", adc_conversion_count);
    printf("传输数据量：%d\n", TOTAL_SAMPLES);
}

// 启动ADC和DMA
void start_adc_dma(void) {
    // 启动DMA
    DMA_SxCR |= DMA_SxCR_EN;
    
    // 启动ADC转换
    printf("启动ADC和DMA采集...\n");
    
    // 模拟连续转换
    for (int i = 0; i < TOTAL_SAMPLES; i++) {
        simulate_adc_conversion_complete();
    }
}

// 打印采样数据
void print_sampled_data(void) {
    printf("\n前5组采样数据（格式：CH0值, CH1值, CH2值）：\n");
    
    for (int group = 0; group < 5; group++) {
        printf("第%d组: ", group + 1);
        for (int ch = 0; ch < ADC_CHANNELS; ch++) {
            int index = group * ADC_CHANNELS + ch;
            if (index < TOTAL_SAMPLES) {
                printf("%d", dma_buffer[index]);
                if (ch < ADC_CHANNELS - 1) {
                    printf(", ");
                }
            }
        }
        printf("\n");
    }
}

// 打印DMA传输统计
void print_dma_stats(void) {
    printf("\nDMA传输统计信息：\n");
    printf("缓冲区地址：%p\n", (void*)dma_buffer);
    printf("缓冲区大小：%d 字节\n", TOTAL_SAMPLES * sizeof(uint16_t));
    printf("剩余传输数量：%d\n", DMA_SxNDTR);
    printf("DMA控制寄存器状态：0x%08X\n", DMA_SxCR);
    
    // 计算数据校验
    uint32_t sum = 0;
    for (int i = 0; i < TOTAL_SAMPLES; i++) {
        sum += dma_buffer[i];
    }
    printf("数据校验和：%u\n", sum);
}

int main(void) {
    // 初始化随机数种子
    srand(time(NULL));
    
    printf("=== STM32 ADC多通道DMA采样模拟程序 ===\n");
    
    // 初始化ADC和DMA
    adc_init();
    dma_init();
    
    // 启动采集
    start_adc_dma();
    
    // 检查传输是否完成
    if (dma_transfer_complete) {
        dma_transfer_complete_callback();
    }
    
    // 打印采样数据
    print_sampled_data();
    
    // 打印DMA统计信息
    print_dma_stats();
    
    printf("\n程序执行完成！\n");
    
    return 0;
}