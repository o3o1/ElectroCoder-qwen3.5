#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define PWM_PERIOD 10
#define PWM_HIGH_TIME 5
#define BITS_PER_BYTE 8
#define CYCLES_PER_BIT 4
#define TIME_UNITS_PER_CYCLE PWM_PERIOD
#define TIME_UNITS_PER_BIT (CYCLES_PER_BIT * TIME_UNITS_PER_CYCLE)
#define MIN_HIGH_THRESHOLD 3

void pwm_encode(uint8_t data, uint16_t *pwm_buffer, size_t buf_size) {
    size_t time_index = 0;
    
    for (int bit = 7; bit >= 0; bit--) {
        uint8_t bit_value = (data >> bit) & 0x01;
        
        for (int cycle = 0; cycle < CYCLES_PER_BIT; cycle++) {
            int should_be_high = 0;
            
            if (bit_value == 1) {
                if (cycle < 3) {
                    should_be_high = 1;
                } else {
                    should_be_high = 0;
                }
            } else {
                if (cycle == 0) {
                    should_be_high = 1;
                } else {
                    should_be_high = 0;
                }
            }
            
            for (int t = 0; t < PWM_PERIOD; t++) {
                if (time_index >= buf_size) return;
                
                if (should_be_high) {
                    pwm_buffer[time_index] = (t < PWM_HIGH_TIME) ? 1 : 0;
                } else {
                    pwm_buffer[time_index] = 0;
                }
                time_index++;
            }
        }
    }
}

uint8_t pwm_decode(const uint16_t *pwm_buffer, size_t buf_size) {
    uint8_t result = 0;
    size_t time_index = 0;
    
    for (int bit = 7; bit >= 0; bit--) {
        int high_count = 0;
        int cycle_high_counts[CYCLES_PER_BIT] = {0};
        
        for (int cycle = 0; cycle < CYCLES_PER_BIT; cycle++) {
            if (time_index >= buf_size) break;
            
            int cycle_high = 0;
            for (int t = 0; t < PWM_PERIOD; t++) {
                if (time_index >= buf_size) break;
                if (pwm_buffer[time_index] == 1) {
                    cycle_high++;
                }
                time_index++;
            }
            cycle_high_counts[cycle] = cycle_high;
        }
        
        int pattern_match_1 = 1;
        int pattern_match_0 = 1;
        
        for (int cycle = 0; cycle < CYCLES_PER_BIT; cycle++) {
            int is_high = (cycle_high_counts[cycle] >= MIN_HIGH_THRESHOLD) ? 1 : 0;
            
            if (cycle < 3) {
                if (is_high != 1) pattern_match_1 = 0;
            } else {
                if (is_high != 0) pattern_match_1 = 0;
            }
            
            if (cycle == 0) {
                if (is_high != 1) pattern_match_0 = 0;
            } else {
                if (is_high != 0) pattern_match_0 = 0;
            }
        }
        
        if (pattern_match_1) {
            result |= (1 << bit);
        } else if (pattern_match_0) {
        } else {
        }
    }
    
    return result;
}

int main() {
    uint8_t test_data = 0x5A;
    size_t buffer_size = BITS_PER_BYTE * TIME_UNITS_PER_BIT;
    uint16_t pwm_buffer[320];
    
    memset(pwm_buffer, 0, sizeof(pwm_buffer));
    pwm_encode(test_data, pwm_buffer, buffer_size);
    
    printf("测试编码功能 (0x5A = 01011010):\n");
    printf("编码后的PWM信号 (前160个时间单位):\n");
    for (int i = 0; i < 160 && i < buffer_size; i++) {
        printf("%d", pwm_buffer[i]);
        if ((i + 1) % 10 == 0) printf(" ");
        if ((i + 1) % 40 == 0) printf("\n");
    }
    printf("\n");
    
    uint8_t decoded_data = pwm_decode(pwm_buffer, buffer_size);
    
    printf("解码结果: 0x%02X\n", decoded_data);
    printf("原始数据: 0x%02X\n", test_data);
    printf("编码解码 %s\n", (decoded_data == test_data) ? "成功" : "失败");
    
    printf("\n模拟通信过程:\n");
    uint8_t tx_data = 0xA5;
    uint16_t tx_buffer[320];
    
    memset(tx_buffer, 0, sizeof(tx_buffer));
    pwm_encode(tx_data, tx_buffer, buffer_size);
    
    printf("原边发送数据: 0x%02X (二进制: ", tx_data);
    for (int i = 7; i >= 0; i--) {
        printf("%d", (tx_data >> i) & 0x01);
    }
    printf(")\n");
    
    uint8_t rx_data = pwm_decode(tx_buffer, buffer_size);
    
    printf("副边接收数据: 0x%02X (二进制: ", rx_data);
    for (int i = 7; i >= 0; i--) {
        printf("%d", (rx_data >> i) & 0x01);
    }
    printf(")\n");
    
    printf("通信 %s\n", (rx_data == tx_data) ? "成功" : "失败");
    
    return 0;
}