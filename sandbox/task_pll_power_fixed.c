#include <stdio.h>
#include <stdint.h>

static uint32_t PLL_CTRL_REG = 0;
static uint32_t PLL_STATUS_REG = 0;
static uint32_t POWER_MODE_REG = 0;

#define PLL_ENABLE_BIT (1 << 0)
#define PLL_SLEEP_REQ_BIT (1 << 1)

#define PLL_LOCKED_BIT (1 << 0)
#define PLL_WAKEUP_DONE_BIT (1 << 1)

#define POWER_MODE_NORMAL 0
#define POWER_MODE_LOW_POWER 1

typedef enum {
    PLL_STATE_ACTIVE,
    PLL_STATE_SLEEP,
    PLL_STATE_WAKING_UP
} PLL_State;

static PLL_State current_state = PLL_STATE_ACTIVE;

void pll_sleep_request(void) {
    PLL_CTRL_REG |= PLL_SLEEP_REQ_BIT;
}

void pll_wakeup_request(void) {
    PLL_CTRL_REG &= ~PLL_SLEEP_REQ_BIT;
}

const char* pll_state_to_string(void) {
    switch(current_state) {
        case PLL_STATE_ACTIVE: return "ACTIVE";
        case PLL_STATE_SLEEP: return "SLEEP";
        case PLL_STATE_WAKING_UP: return "WAKING_UP";
        default: return "UNKNOWN";
    }
}

void pll_state_machine(void) {
    switch(current_state) {
        case PLL_STATE_ACTIVE:
            if ((PLL_STATUS_REG & PLL_LOCKED_BIT) && (PLL_CTRL_REG & PLL_SLEEP_REQ_BIT)) {
                current_state = PLL_STATE_SLEEP;
                POWER_MODE_REG = POWER_MODE_LOW_POWER;
            }
            break;
            
        case PLL_STATE_SLEEP:
            if (!(PLL_CTRL_REG & PLL_SLEEP_REQ_BIT)) {
                current_state = PLL_STATE_WAKING_UP;
                PLL_STATUS_REG &= ~PLL_WAKEUP_DONE_BIT;
            }
            break;
            
        case PLL_STATE_WAKING_UP:
            if (PLL_STATUS_REG & PLL_WAKEUP_DONE_BIT) {
                current_state = PLL_STATE_ACTIVE;
                POWER_MODE_REG = POWER_MODE_NORMAL;
            }
            break;
    }
}

int main() {
    PLL_CTRL_REG = 0;
    PLL_STATUS_REG = 0;
    POWER_MODE_REG = 0;
    current_state = PLL_STATE_ACTIVE;
    
    printf("=== PLL低功耗模式管理模拟 ===\n\n");
    
    printf("1. 初始化状态:\n");
    printf("   当前状态: %s\n", pll_state_to_string());
    printf("   PLL_CTRL_REG: 0x%08X\n", PLL_CTRL_REG);
    printf("   PLL_STATUS_REG: 0x%08X\n", PLL_STATUS_REG);
    printf("   POWER_MODE_REG: 0x%08X\n\n", POWER_MODE_REG);
    
    printf("2. 使能PLL并等待锁定:\n");
    PLL_CTRL_REG |= PLL_ENABLE_BIT;
    PLL_STATUS_REG |= PLL_LOCKED_BIT;
    pll_state_machine();
    printf("   当前状态: %s\n", pll_state_to_string());
    printf("   PLL_CTRL_REG: 0x%08X\n", PLL_CTRL_REG);
    printf("   PLL_STATUS_REG: 0x%08X\n\n", PLL_STATUS_REG);
    
    printf("3. 请求休眠:\n");
    pll_sleep_request();
    pll_state_machine();
    printf("   当前状态: %s\n", pll_state_to_string());
    printf("   PLL_CTRL_REG: 0x%08X\n", PLL_CTRL_REG);
    printf("   POWER_MODE_REG: 0x%08X\n\n", POWER_MODE_REG);
    
    printf("4. 请求唤醒:\n");
    pll_wakeup_request();
    pll_state_machine();
    printf("   当前状态: %s\n", pll_state_to_string());
    printf("   PLL_CTRL_REG: 0x%08X\n\n", PLL_CTRL_REG);
    
    printf("5. 完成唤醒过程:\n");
    PLL_STATUS_REG |= PLL_WAKEUP_DONE_BIT;
    pll_state_machine();
    printf("   当前状态: %s\n", pll_state_to_string());
    printf("   PLL_STATUS_REG: 0x%08X\n", PLL_STATUS_REG);
    printf("   POWER_MODE_REG: 0x%08X\n\n", POWER_MODE_REG);
    
    printf("=== 模拟完成 ===\n");
    
    return 0;
}