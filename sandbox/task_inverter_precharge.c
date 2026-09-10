#include <stdio.h>
#include <stdint.h>

// 模拟硬件寄存器
volatile uint32_t DC_BUS_VOLTAGE = 50;  // 初始电压50V
volatile uint8_t PRECHARGE_RELAY = 0;   // 预充电继电器控制寄存器
volatile uint8_t MAIN_CONTACTOR = 0;    // 主接触器控制寄存器

// 定义常量
#define RATED_VOLTAGE 310
#define THRESHOLD_RATIO 0.85f
#define START_THRESHOLD (RATED_VOLTAGE * THRESHOLD_RATIO)

void inverter_precharge_control(void) {
    // 读取当前直流母线电压
    uint32_t current_voltage = DC_BUS_VOLTAGE;
    
    // 计算阈值电压
    uint32_t threshold_voltage = (uint32_t)(RATED_VOLTAGE * THRESHOLD_RATIO);
    
    // 打印当前电压
    printf("当前直流母线电压: %uV, 阈值电压: %uV\n", current_voltage, threshold_voltage);
    
    if (current_voltage < threshold_voltage) {
        // 电压低于阈值，闭合预充电继电器，断开主接触器
        PRECHARGE_RELAY = 1;
        MAIN_CONTACTOR = 0;
        printf("状态: 预充电中... (PRECHARGE_RELAY=%d, MAIN_CONTACTOR=%d)\n", 
               PRECHARGE_RELAY, MAIN_CONTACTOR);
    } else {
        // 电压达到或超过阈值，断开预充电继电器，闭合主接触器
        PRECHARGE_RELAY = 0;
        MAIN_CONTACTOR = 1;
        printf("状态: 预充电完成，主接触器闭合 (PRECHARGE_RELAY=%d, MAIN_CONTACTOR=%d)\n", 
               PRECHARGE_RELAY, MAIN_CONTACTOR);
    }
}

int main() {
    printf("=== 逆变器预充电控制模拟测试 ===\n");
    printf("额定电压: %dV, 阈值比例: %.2f, 启动阈值: %.0fV\n\n", 
           RATED_VOLTAGE, THRESHOLD_RATIO, RATED_VOLTAGE * THRESHOLD_RATIO);
    
    // 测试场景1: 初始电压50V（低于阈值）
    printf("测试1: 初始状态 (电压=50V)\n");
    DC_BUS_VOLTAGE = 50;
    inverter_precharge_control();
    printf("\n");
    
    // 测试场景2: 模拟预充电过程，电压升至200V（仍低于阈值）
    printf("测试2: 预充电过程中 (电压=200V)\n");
    DC_BUS_VOLTAGE = 200;
    inverter_precharge_control();
    printf("\n");
    
    // 测试场景3: 电压升至300V（高于阈值）
    printf("测试3: 预充电完成 (电压=300V)\n");
    DC_BUS_VOLTAGE = 300;
    inverter_precharge_control();
    printf("\n");
    
    // 测试场景4: 电压刚好达到阈值263.5V（四舍五入为264V）
    printf("测试4: 电压达到阈值 (电压=264V)\n");
    DC_BUS_VOLTAGE = 264;  // 310 * 0.85 = 263.5
    inverter_precharge_control();
    printf("\n");
    
    printf("=== 测试完成 ===\n");
    
    return 0;
}