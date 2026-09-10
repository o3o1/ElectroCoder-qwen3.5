#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

void pwm_encode(uint8_t data, uint32_t pwm_period, uint32_t *pwm_buffer, size_t buffer_size) {
    if (buffer_size < 16) return;
    
    uint32_t base_width = pwm_period / 2;
    uint32_t delta = pwm_period / 4;
    
    for (int i = 0; i < 8; i++) {
        uint8_t bit = (data >> (7 - i)) & 0x01;
        uint32_t width = bit ? (base_width + delta) : (base_width - delta);
        pwm_buffer[i * 2] = width;
        pwm_buffer[i * 2 + 1] = width;
    }
}

uint8_t pwm_decode(const uint32_t *pwm_buffer, size_t buffer_size, uint32_t pwm_period) {
    if (buffer_size < 16) return 0;
    
    uint8_t result = 0;
    uint32_t base_width = pwm_period / 2;
    
    for (int i = 0; i < 8; i++) {
        uint32_t width = pwm_buffer[i * 2];
        int32_t diff = (int32_t)width - (int32_t)base_width;
        uint8_t bit = (diff > 0) ? 1 : 0;
        result = (result << 1) | bit;
    }
    
    return result;
}

int main() {
    uint32_t pwm_period = 1000;
    uint32_t buffer[16];
    
    printf("Test case 1: Encode data 0xAA\n");
    pwm_encode(0xAA, pwm_period, buffer, 16);
    printf("Encoded PWM high level width: ");
    for (int i = 0; i < 16; i++) {
        printf("%u ", buffer[i]);
    }
    printf("\n");
    
    printf("\nTest case 2: Encode data 0x55\n");
    pwm_encode(0x55, pwm_period, buffer, 16);
    printf("Encoded PWM high level width: ");
    for (int i = 0; i < 16; i++) {
        printf("%u ", buffer[i]);
    }
    printf("\n");
    
    printf("\nTest case 3: Decode 0xAA encoded data\n");
    pwm_encode(0xAA, pwm_period, buffer, 16);
    uint8_t decoded = pwm_decode(buffer, 16, pwm_period);
    printf("Decoded result: 0x%02X (should be 0xAA)\n", decoded);
    
    printf("\nTest case 4: Decode 0x55 encoded data\n");
    pwm_encode(0x55, pwm_period, buffer, 16);
    decoded = pwm_decode(buffer, 16, pwm_period);
    printf("Decoded result: 0x%02X (should be 0x55)\n", decoded);
    
    return 0;
}