#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define POWER_NORMAL 1
#define POWER_FAILURE 0

typedef struct {
    int feed_counter;
    int timeout_threshold;
    int power_status;
} Watchdog;

void watchdog_init(Watchdog *wd, int threshold) {
    wd->feed_counter = 0;
    wd->timeout_threshold = threshold;
    wd->power_status = POWER_NORMAL;
}

void watchdog_feed(Watchdog *wd) {
    wd->feed_counter = 0;
}

int check_power_failure(int cycle) {
    if (cycle == 3) {
        return POWER_FAILURE;
    }
    return (rand() % 10) > 1 ? POWER_NORMAL : POWER_FAILURE;
}

void system_reboot(Watchdog *wd) {
    printf("触发系统重启...\n");
    watchdog_init(wd, wd->timeout_threshold);
}

int main() {
    srand(time(NULL));
    
    Watchdog wd;
    watchdog_init(&wd, 3);
    
    for (int cycle = 1; cycle <= 5; cycle++) {
        printf("周期%d: ", cycle);
        
        wd.power_status = check_power_failure(cycle);
        
        if (wd.power_status == POWER_NORMAL) {
            watchdog_feed(&wd);
            printf("电源正常 | 喂狗成功 | 计数器:%d\n", wd.feed_counter);
        } else {
            printf("电源故障！");
            system_reboot(&wd);
        }
        
        wd.feed_counter++;
        
        if (wd.feed_counter >= wd.timeout_threshold) {
            printf("看门狗超时！");
            system_reboot(&wd);
        }
    }
    
    return 0;
}