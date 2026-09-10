#include <stdio.h>
#include <math.h>
#include <stdlib.h>

double trapezoidal_integral(double x[], double y[], int n) {
    if (n < 2) return 0.0;
    
    double integral = 0.0;
    for (int i = 1; i < n; i++) {
        double dx = x[i] - x[i - 1];
        double avg_y = (y[i] + y[i - 1]) / 2.0;
        integral += avg_y * dx;
    }
    return integral;
}

void test_constant_function() {
    printf("=== 测试1: 常数函数 y=2.0 在区间[0,5] ===\n");
    
    int n = 6;
    double x[] = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0};
    double y[] = {2.0, 2.0, 2.0, 2.0, 2.0, 2.0};
    
    printf("时间数组 x: ");
    for (int i = 0; i < n; i++) printf("%.1f ", x[i]);
    printf("\n信号数组 y: ");
    for (int i = 0; i < n; i++) printf("%.1f ", y[i]);
    printf("\n");
    
    double result = trapezoidal_integral(x, y, n);
    double expected = 10.0;
    double error = fabs(result - expected);
    
    printf("计算结果: %.6f\n", result);
    printf("理论值: %.6f\n", expected);
    printf("误差: %.6f\n\n", error);
}

void test_linear_function() {
    printf("=== 测试2: 线性函数 y=x 在区间[0,3] ===\n");
    
    int n = 7;
    double x[] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0};
    double y[] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0};
    
    printf("时间数组 x: ");
    for (int i = 0; i < n; i++) printf("%.1f ", x[i]);
    printf("\n信号数组 y: ");
    for (int i = 0; i < n; i++) printf("%.1f ", y[i]);
    printf("\n");
    
    double result = trapezoidal_integral(x, y, n);
    double expected = 4.5;
    double error = fabs(result - expected);
    
    printf("计算结果: %.6f\n", result);
    printf("理论值: %.6f\n", expected);
    printf("误差: %.6f\n\n", error);
}

void test_sine_function() {
    printf("=== 测试3: 正弦函数 y=sin(x) 在区间[0,π] ===\n");
    
    int n = 11;
    double x[n];
    double y[n];
    
    for (int i = 0; i < n; i++) {
        x[i] = i * M_PI / (n - 1);
        y[i] = sin(x[i]);
    }
    
    printf("时间数组 x: ");
    for (int i = 0; i < n; i++) printf("%.3f ", x[i]);
    printf("\n信号数组 y: ");
    for (int i = 0; i < n; i++) printf("%.3f ", y[i]);
    printf("\n");
    
    double result = trapezoidal_integral(x, y, n);
    double expected = 2.0;
    double error = fabs(result - expected);
    
    printf("计算结果: %.6f\n", result);
    printf("理论值: %.6f\n", expected);
    printf("误差: %.6f\n\n", error);
}

int main() {
    printf("电力电子信号积分计算 - 梯形积分法\n");
    printf("===================================\n\n");
    
    test_constant_function();
    test_linear_function();
    test_sine_function();
    
    return 0;
}