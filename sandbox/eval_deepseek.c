#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// 模拟硬件寄存器宏定义
#define REG_PWM_DUTY_A      (*(volatile uint32_t*)0x40000000)
#define REG_PWM_DUTY_B      (*(volatile uint32_t*)0x40000004)
#define REG_PWM_DUTY_C      (*(volatile uint32_t*)0x40000008)
#define REG_ADC_BUS_VOLTAGE (*(volatile uint32_t*)0x4000000C)
#define REG_ADC_CAP_VOLTAGE (*(volatile uint32_t*)0x40000010)
#define REG_CONTROL_STATUS  (*(volatile uint32_t*)0x40000014)
#define REG_TIMER_COUNTER   (*(volatile uint32_t*)0x40000018)

// 控制状态寄存器位定义
#define STATUS_PRE_CHARGE_DONE  (1 << 0)
#define STATUS_FAULT            (1 << 1)
#define STATUS_ENABLE           (1 << 2)

// 预充电参数
#define PRE_CHARGE_TARGET_VOLTAGE   350  // 目标电压 (V)
#define PRE_CHARGE_CURRENT_LIMIT    10   // 电流限制 (A)
#define PRE_CHARGE_DUTY_STEP        50   // 占空比步进值
#define PRE_CHARGE_DUTY_MAX         1000 // 最大占空比 (0.1% 分辨率)
#define PRE_CHARGE_TIMEOUT_MS       5000 // 超时时间 (ms)

// 状态机枚举
typedef enum {
    PRE_CHARGE_IDLE,
    PRE_CHARGE_ACTIVE,
    PRE_CHARGE_COMPLETE,
    PRE_CHARGE_FAULT
} PreChargeState_t;

// 预充电控制结构体
typedef struct {
    PreChargeState_t state;
    uint32_t current_duty;
    uint32_t target_duty;
    uint32_t bus_voltage;
    uint32_t cap_voltage;
    uint32_t timer_count;
    bool     timeout_flag;
} PreChargeController_t;

// 全局预充电控制器
static PreChargeController_t precharge_ctrl = {
    .state = PRE_CHARGE_IDLE,
    .current_duty = 0,
    .target_duty = 0,
    .bus_voltage = 0,
    .cap_voltage = 0,
    .timer_count = 0,
    .timeout_flag = false
};

// 模拟读取ADC电压
static uint32_t read_adc_bus_voltage(void) {
    // 模拟母线电压为400V
    return 400;
}

static uint32_t read_adc_cap_voltage(void) {
    // 模拟电容电压逐渐上升
    static uint32_t cap_voltage = 0;
    if (cap_voltage < 350) {
        cap_voltage += 5;
    }
    return cap_voltage;
}

// 设置PWM占空比
static void set_pwm_duty(uint32_t duty_a, uint32_t duty_b, uint32_t duty_c) {
    REG_PWM_DUTY_A = duty_a;
    REG_PWM_DUTY_B = duty_b;
    REG_PWM_DUTY_C = duty_c;
}

// 预充电状态机处理
static void pre_charge_process(void) {
    switch (precharge_ctrl.state) {
        case PRE_CHARGE_IDLE:
            // 检查是否满足启动条件
            if (REG_CONTROL_STATUS & STATUS_ENABLE) {
                precharge_ctrl.state = PRE_CHARGE_ACTIVE;
                precharge_ctrl.current_duty = 0;
                precharge_ctrl.target_duty = PRE_CHARGE_DUTY_MAX;
                precharge_ctrl.timer_count = 0;
                precharge_ctrl.timeout_flag = false;
                printf("Pre-charge started\n");
            }
            break;

        case PRE_CHARGE_ACTIVE:
            // 读取电压
            precharge_ctrl.bus_voltage = read_adc_bus_voltage();
            precharge_ctrl.cap_voltage = read_adc_cap_voltage();

            // 超时检测
            precharge_ctrl.timer_count++;
            if (precharge_ctrl.timer_count >= PRE_CHARGE_TIMEOUT_MS) {
                precharge_ctrl.timeout_flag = true;
                precharge_ctrl.state = PRE_CHARGE_FAULT;
                printf("Pre-charge timeout!\n");
                break;
            }

            // 占空比斜坡上升
            if (precharge_ctrl.current_duty < precharge_ctrl.target_duty) {
                precharge_ctrl.current_duty += PRE_CHARGE_DUTY_STEP;
                if (precharge_ctrl.current_duty > precharge_ctrl.target_duty) {
                    precharge_ctrl.current_duty = precharge_ctrl.target_duty;
                }
            }

            // 应用占空比
            set_pwm_duty(precharge_ctrl.current_duty, 
                        precharge_ctrl.current_duty, 
                        precharge_ctrl.current_duty);

            // 检查电容电压是否达到目标
            if (precharge_ctrl.cap_voltage >= PRE_CHARGE_TARGET_VOLTAGE) {
                precharge_ctrl.state = PRE_CHARGE_COMPLETE;
                printf("Pre-charge complete! Cap voltage: %d V\n", precharge_ctrl.cap_voltage);
            }

            printf("Pre-charging: duty=%d, cap=%d V, bus=%d V\n", 
                   precharge_ctrl.current_duty, 
                   precharge_ctrl.cap_voltage, 
                   precharge_ctrl.bus_voltage);
            break;

        case PRE_CHARGE_COMPLETE:
            // 保持占空比，设置完成标志
            REG_CONTROL_STATUS |= STATUS_PRE_CHARGE_DONE;
            break;

        case PRE_CHARGE_FAULT:
            // 故障处理：关闭PWM
            set_pwm_duty(0, 0, 0);
            REG_CONTROL_STATUS |= STATUS_FAULT;
            printf("Fault state - PWM disabled\n");
            break;
    }
}

// 初始化预充电控制器
static void pre_charge_init(void) {
    precharge_ctrl.state = PRE_CHARGE_IDLE;
    precharge_ctrl.current_duty = 0;
    precharge_ctrl.target_duty = 0;
    precharge_ctrl.bus_voltage = 0;
    precharge_ctrl.cap_voltage = 0;
    precharge_ctrl.timer_count = 0;
    precharge_ctrl.timeout_flag = false;
    
    // 初始化硬件寄存器
    REG_CONTROL_STATUS = 0;
    REG_PWM_DUTY_A = 0;
    REG_PWM_DUTY_B = 0;
    REG_PWM_DUTY_C = 0;
    
    printf("Pre-charge controller initialized\n");
}

int main(void) {
    printf("Inverter Low Voltage Start (Pre-charge) Control Demo\n");
    printf("===================================================\n\n");
    
    // 初始化
    pre_charge_init();
    
    // 模拟使能信号
    REG_CONTROL_STATUS |= STATUS_ENABLE;
    printf("Enable signal asserted\n\n");
    
    // 主循环模拟
    for (int i = 0; i < 100; i++) {
        pre_charge_process();
        
        // 模拟10ms定时中断
        // 实际应用中这里应该是定时器中断或RTOS延时
        // 这里使用简单的循环模拟时间流逝
        for (volatile int j = 0; j < 100000; j++);
        
        // 如果完成或故障则退出循环
        if (precharge_ctrl.state == PRE_CHARGE_COMPLETE || 
            precharge_ctrl.state == PRE_CHARGE_FAULT) {
            break;
        }
    }
    
    printf("\nFinal state: ");
    switch (precharge_ctrl.state) {
        case PRE_CHARGE_IDLE:
            printf("IDLE\n");
            break;
        case PRE_CHARGE_ACTIVE:
            printf("ACTIVE (not completed)\n");
            break;
        case PRE_CHARGE_COMPLETE:
            printf("COMPLETE\n");
            break;
        case PRE_CHARGE_FAULT:
            printf("FAULT\n");
            break;
    }
    
    return 0;
}