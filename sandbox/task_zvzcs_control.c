#include <stdio.h>
#include <math.h>

void calculate_phase_shift_timing(double Vin, double Iout, double fsw, double *t_delay, double *t_resonant) {
    double t_max = 0.5 / fsw;
    
    double t_delay_calc = (0.3 * Iout) / (Vin * fsw) + (1e-6);
    if (t_delay_calc > t_max) {
        t_delay_calc = t_max;
    }
    if (t_delay_calc < 0) {
        t_delay_calc = 0;
    }
    
    double t_resonant_calc = 0.25 / fsw + (0.1 * Iout / Vin) * (1e-6);
    if (t_resonant_calc > t_max) {
        t_resonant_calc = t_max;
    }
    if (t_resonant_calc < 0) {
        t_resonant_calc = 0;
    }
    
    *t_delay = t_delay_calc;
    *t_resonant = t_resonant_calc;
}

int main() {
    struct TestCase {
        double Vin;
        double Iout;
        double fsw;
    };
    
    struct TestCase test_cases[] = {
        {400.0, 10.0, 100000.0},
        {300.0, 25.0, 150000.0},
        {500.0, 5.0, 80000.0}
    };
    
    int num_tests = sizeof(test_cases) / sizeof(test_cases[0]);
    
    printf("ZVZCS移相全桥辅助网络软开关时序计算\n");
    printf("========================================\n");
    
    for (int i = 0; i < num_tests; i++) {
        double Vin = test_cases[i].Vin;
        double Iout = test_cases[i].Iout;
        double fsw = test_cases[i].fsw;
        
        double t_delay, t_resonant;
        calculate_phase_shift_timing(Vin, Iout, fsw, &t_delay, &t_resonant);
        
        printf("\n测试用例 %d:\n", i + 1);
        printf("输入电压 Vin = %.1f V\n", Vin);
        printf("输出电流 Iout = %.1f A\n", Iout);
        printf("开关频率 fsw = %.0f Hz (%.1f kHz)\n", fsw, fsw / 1000.0);
        printf("移相延迟时间 t_delay = %.3f us\n", t_delay * 1e6);
        printf("辅助网络谐振时间 t_resonant = %.3f us\n", t_resonant * 1e6);
        printf("半个开关周期 t_max = %.3f us\n", (0.5 / fsw) * 1e6);
    }
    
    return 0;
}