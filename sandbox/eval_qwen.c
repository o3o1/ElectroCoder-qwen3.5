#include <stdio.h>

// Mock 定义高压电机控制器寄存器
#define MOTOR_CTRL_REG1 (*(volatile unsigned int *)0x40000000)
#define MOTOR_CTRL_REG2 (*(volatile unsigned int *)0x40000004)

// 模拟电机电流反馈
float motor_current = 0.0;

// FOC 控制算法函数
void foc_control() {
    // 这里可以实现 FOC 算法的具体逻辑
    // 例如：计算转子位置、电流矢量等

    // 示例：简单地增加电机电流模拟
    motor_current += 0.1;
}

int main() {
    while (1) {
        foc_control();
        printf("Motor Current: %.2f A\n", motor_current);
    }

    return 0;
}