#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/* ================= Mock 寄存器与硬件抽象 ================= */
/* 使用普通变量模拟寄存器，避免非法地址访问导致段错误 */
static volatile uint16_t mock_adc_data_reg = 0U;
static volatile uint16_t mock_dac_data_reg = 0U;
static volatile uint32_t mock_gpio_out_reg = 0U;
static volatile uint32_t mock_systick_ms   = 0U;
static volatile uint8_t  mock_adc_ch_sel   = 0U;

#define MOCK_ADC_DATA_REG   (mock_adc_data_reg)
#define MOCK_DAC_DATA_REG   (mock_dac_data_reg)
#define MOCK_GPIO_OUT_REG   (mock_gpio_out_reg)
#define MOCK_SYSTICK_MS     (mock_systick_ms)
#define MOCK_ADC_CH_SEL     (mock_adc_ch_sel)

/* ADC 通道定义 */
#define ADC_CH_TEMP             0U
#define ADC_CH_HUMI             1U
#define ADC_CH_H2S              2U
#define ADC_CH_SO2              3U

/* GPIO 位定义 */
#define GPIO_BIT_FAN            0U
#define GPIO_BIT_HEATER         1U
#define GPIO_BIT_ALARM          2U
#define GPIO_BIT_LOAD_SHED      3U

/* ================= 系统参数配置 ================= */
#define ADC_VREF_MV             3300U
#define ADC_FULL_SCALE          4095U

#define TEMP_HIGH_LIMIT         600
#define TEMP_LOW_LIMIT          50
#define TEMP_HYSTERESIS         20

#define HUMI_HIGH_LIMIT         850
#define HUMI_HYSTERESIS         50

#define H2S_HIGH_LIMIT          20
#define SO2_HIGH_LIMIT          15
#define GAS_HYSTERESIS          3

#define LOAD_LEVEL_FULL         100U
#define LOAD_LEVEL_REDUCE_1     70U
#define LOAD_LEVEL_REDUCE_2     40U
#define LOAD_LEVEL_SHUTDOWN     0U

#define CONTROL_PERIOD_MS       100U

/* ================= 数据结构 ================= */
typedef struct {
    int16_t  temperature;
    uint16_t humidity;
    uint16_t h2s_ppm;
    uint16_t so2_ppm;
} EnvData_t;

typedef struct {
    uint8_t  load_percent;
    bool     fan_on;
    bool     heater_on;
    bool     alarm_on;
    bool     load_shed_on;
    uint8_t  shed_level;
} CtrlState_t;

/* ================= 传感器读取（模拟） ================= */
static uint16_t Mock_ReadAdc(uint8_t channel)
{
    switch (channel) {
        case ADC_CH_TEMP: return 2500U;
        case ADC_CH_HUMI: return 3000U;
        case ADC_CH_H2S:  return 1500U;
        case ADC_CH_SO2:  return 2000U;
        default:          return 0U;
    }
}

static int16_t ConvertToTemperature(uint16_t adc_raw)
{
    int32_t mv = ((int32_t)adc_raw * (int32_t)ADC_VREF_MV) / (int32_t)ADC_FULL_SCALE;
    int32_t temp = -400 + (mv * 1650) / 3300;
    return (int16_t)temp;
}

static uint16_t ConvertToHumidity(uint16_t adc_raw)
{
    uint32_t rh = ((uint32_t)adc_raw * 1000U) / (uint32_t)ADC_FULL_SCALE;
    return (uint16_t)rh;
}

static uint16_t ConvertToGasPpm(uint16_t adc_raw)
{
    uint32_t ppm = ((uint32_t)adc_raw * 100U) / (uint32_t)ADC_FULL_SCALE;
    return (uint16_t)ppm;
}

/* ================= 环境数据采集 ================= */
static void Env_Acquire(EnvData_t *env)
{
    uint16_t raw;

    MOCK_ADC_CH_SEL = ADC_CH_TEMP;
    raw = Mock_ReadAdc(ADC_CH_TEMP);
    env->temperature = ConvertToTemperature(raw);

    MOCK_ADC_CH_SEL = ADC_CH_HUMI;
    raw = Mock_ReadAdc(ADC_CH_HUMI);
    env->humidity = ConvertToHumidity(raw);

    MOCK_ADC_CH_SEL = ADC_CH_H2S;
    raw = Mock_ReadAdc(ADC_CH_H2S);
    env->h2s_ppm = ConvertToGasPpm(raw);

    MOCK_ADC_CH_SEL = ADC_CH_SO2;
    raw = Mock_ReadAdc(ADC_CH_SO2);
    env->so2_ppm = ConvertToGasPpm(raw);
}

/* ================= 降载决策算法 ================= */
static uint8_t Env_DecideShedLevel(const EnvData_t *env, uint8_t prev_level)
{
    uint8_t level = 0U;

    if (env->temperature >= TEMP_HIGH_LIMIT) {
        level = 2U;
    } else if (env->temperature >= (TEMP_HIGH_LIMIT - TEMP_HYSTERESIS)) {
        if (prev_level >= 1U) {
            level = 1U;
        }
    }

    if (env->humidity >= HUMI_HIGH_LIMIT) {
        if (level < 2U) level = 2U;
    } else if (env->humidity >= (HUMI_HIGH_LIMIT - HUMI_HYSTERESIS)) {
        if (prev_level >= 1U && level < 1U) level = 1U;
    }

    if (env->h2s_ppm >= H2S_HIGH_LIMIT) {
        if (level < 2U) level = 2U;
    } else if (env->h2s_ppm >= (H2S_HIGH_LIMIT - GAS_HYSTERESIS)) {
        if (prev_level >= 1U && level < 1U) level = 1U;
    }

    if (env->so2_ppm >= SO2_HIGH_LIMIT) {
        if (level < 2U) level = 2U;
    } else if (env->so2_ppm >= (SO2_HIGH_LIMIT - GAS_HYSTERESIS)) {
        if (prev_level >= 1U && level < 1U) level = 1U;
    }

    if ((env->temperature >= (TEMP_HIGH_LIMIT + 100)) ||
        (env->h2s_ppm >= (H2S_HIGH_LIMIT * 2U)) ||
        (env->so2_ppm >= (SO2_HIGH_LIMIT * 2U))) {
        level = 3U;
    }

    return level;
}

/* ================= 执行机构控制 ================= */
static void Ctrl_Apply(const EnvData_t *env, CtrlState_t *ctrl)
{
    uint8_t shed_level;

    shed_level = Env_DecideShedLevel(env, ctrl->shed_level);
    ctrl->shed_level = shed_level;

    switch (shed_level) {
        case 0U: ctrl->load_percent = LOAD_LEVEL_FULL;     break;
        case 1U: ctrl->load_percent = LOAD_LEVEL_REDUCE_1; break;
        case 2U: ctrl->load_percent = LOAD_LEVEL_REDUCE_2; break;
        case 3U: ctrl->load_percent = LOAD_LEVEL_SHUTDOWN; break;
        default: ctrl->load_percent = LOAD_LEVEL_FULL;     break;
    }

    if (env->temperature >= 450) {
        ctrl->fan_on = true;
    } else if (env->temperature <= 400) {
        ctrl->fan_on = false;
    }

    if ((env->humidity >= 800U) && (env->temperature <= 300)) {
        ctrl->heater_on = true;
    } else if ((env->humidity <= 750U) || (env->temperature >= 350)) {
        ctrl->heater_on = false;
    }

    if ((env->temperature >= TEMP_HIGH_LIMIT) ||
        (env->humidity >= HUMI_HIGH_LIMIT) ||
        (env->h2s_ppm >= H2S_HIGH_LIMIT) ||
        (env->so2_ppm >= SO2_HIGH_LIMIT)) {
        ctrl->alarm_on = true;
    } else {
        ctrl->alarm_on = false;
    }

    ctrl->load_shed_on = (shed_level > 0U);

    {
        uint32_t gpio = MOCK_GPIO_OUT_REG;
        if (ctrl->fan_on)       gpio |= (1U << GPIO_BIT_FAN);
        else                    gpio &= ~(1U << GPIO_BIT_FAN);

        if (ctrl->heater_on)    gpio |= (1U << GPIO_BIT_HEATER);
        else                    gpio &= ~(1U << GPIO_BIT_HEATER);

        if (ctrl->alarm_on)     gpio |= (1U << GPIO_BIT_ALARM);
        else                    gpio &= ~(1U << GPIO_BIT_ALARM);

        if (ctrl->load_shed_on) gpio |= (1U << GPIO_BIT_LOAD_SHED);
        else                    gpio &= ~(1U << GPIO_BIT_LOAD_SHED);

        MOCK_GPIO_OUT_REG = gpio;
    }

    MOCK_DAC_DATA_REG = (uint16_t)((uint32_t)ctrl->load_percent * ADC_FULL_SCALE / 100U);
}

/* ================= 状态打印 ================= */
static void Print_Status(const EnvData_t *env, const CtrlState_t *ctrl)
{
    int16_t t = env->temperature;
    int16_t t_abs = (t < 0) ? (int16_t)(-t) : t;

    printf("========== 化工电源防腐控制状态 ==========\n");
    printf("环境参数:\n");
    printf("  温度:     %s%d.%d C\n", (t < 0) ? "-" : "", t_abs / 10, t_abs % 10);
    printf("  湿度:     %u.%u %%RH\n", env->humidity / 10, env->humidity % 10);
    printf("  H2S:      %u ppm\n", env->h2s_ppm);
    printf("  SO2:      %u ppm\n", env->so2_ppm);
    printf("控制输出:\n");
    printf("  负载:     %u %%\n", ctrl->load_percent);
    printf("  降载等级: %u (0=无,1=一级,2=二级,3=停机)\n", ctrl->shed_level);
    printf("  风扇:     %s\n", ctrl->fan_on ? "开" : "关");
    printf("  加热除湿: %s\n", ctrl->heater_on ? "开" : "关");
    printf("  报警:     %s\n", ctrl->alarm_on ? "是" : "否");
    printf("  降载继电器: %s\n", ctrl->load_shed_on ? "吸合" : "断开");
    printf("==========================================\n\n");
}

/* ================= 主函数 ================= */
int main(void)
{
    EnvData_t env;
    CtrlState_t ctrl;
    int cycle;

    ctrl.load_percent = LOAD_LEVEL_FULL;
    ctrl.fan_on       = false;
    ctrl.heater_on    = false;
    ctrl.alarm_on     = false;
    ctrl.load_shed_on = false;
    ctrl.shed_level   = 0U;

    env.temperature = 0;
    env.humidity    = 0U;
    env.h2s_ppm     = 0U;
    env.so2_ppm     = 0U;

    printf("化工电源防腐控制系统启动...\n\n");

    for (cycle = 0; cycle < 5; cycle++) {
        printf(">>> 控制周期 %d <<<\n", cycle + 1);

        Env_Acquire(&env);
        Ctrl_Apply(&env, &ctrl);
        Print_Status(&env, &ctrl);

        MOCK_SYSTICK_MS += CONTROL_PERIOD_MS;
    }

    printf("系统运行结束。\n");
    return 0;
}