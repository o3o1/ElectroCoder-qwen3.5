#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// 模糊集合定义
typedef enum {
    NB = 0,  // 负大
    NS = 1,  // 负小
    ZO = 2,  // 零
    PS = 3,  // 正小
    PB = 4   // 正大
} FuzzySet;

// PID参数结构体
typedef struct {
    float Kp;
    float Ki;
    float Kd;
} PIDParams;

// 模糊控制器结构体
typedef struct {
    // 输入变量范围
    float e_min;
    float e_max;
    float ec_min;
    float ec_max;
    
    // 输出变量范围
    float deltaKp_min;
    float deltaKp_max;
    
    // 模糊规则表
    FuzzySet rule_table[5][5];
} FuzzyController;

// 初始化模糊控制器
void init_fuzzy_controller(FuzzyController* fc) {
    fc->e_min = -50.0f;
    fc->e_max = 50.0f;
    fc->ec_min = -10.0f;
    fc->ec_max = 10.0f;
    fc->deltaKp_min = -0.5f;
    fc->deltaKp_max = 0.5f;
    
    // 初始化规则表
    FuzzySet table[5][5] = {
        {ZO, PS, PB, PB, PB},
        {NS, ZO, PS, PB, PB},
        {NB, NS, ZO, PS, PB},
        {NB, NB, NS, ZO, PS},
        {NB, NB, NB, NS, ZO}
    };
    
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            fc->rule_table[i][j] = table[i][j];
        }
    }
}

// 三角形隶属度函数
float triangle_membership(float x, float a, float b, float c) {
    if (x <= a || x >= c) return 0.0f;
    if (x > a && x <= b) return (x - a) / (b - a);
    if (x > b && x < c) return (c - x) / (c - b);
    return 0.0f;
}

// 模糊化：计算输入值对每个模糊集合的隶属度
void fuzzify_input(float value, float min_val, float max_val, float* membership) {
    // 归一化到[-2, 2]范围，对应5个模糊集合
    float normalized = (value - min_val) / (max_val - min_val) * 4.0f - 2.0f;
    
    // 计算每个模糊集合的隶属度（三角形分布）
    membership[NB] = triangle_membership(normalized, -3.0f, -2.0f, -1.0f);
    membership[NS] = triangle_membership(normalized, -2.0f, -1.0f, 0.0f);
    membership[ZO] = triangle_membership(normalized, -1.0f, 0.0f, 1.0f);
    membership[PS] = triangle_membership(normalized, 0.0f, 1.0f, 2.0f);
    membership[PB] = triangle_membership(normalized, 1.0f, 2.0f, 3.0f);
}

// 模糊推理：根据规则表计算输出隶属度
void fuzzy_inference(const FuzzyController* fc, 
                     const float* e_membership, 
                     const float* ec_membership,
                     float* output_membership) {
    // 初始化输出隶属度为0
    for (int i = 0; i < 5; i++) {
        output_membership[i] = 0.0f;
    }
    
    // 遍历所有规则
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            // 规则激活度 = min(e隶属度, ec隶属度)
            float activation = e_membership[i] < ec_membership[j] ? 
                              e_membership[i] : ec_membership[j];
            
            if (activation > 0) {
                // 获取规则结论
                FuzzySet conclusion = fc->rule_table[i][j];
                
                // 使用max-min推理
                if (activation > output_membership[conclusion]) {
                    output_membership[conclusion] = activation;
                }
            }
        }
    }
}

// 去模糊化：使用重心法计算精确输出值
float defuzzify(const FuzzyController* fc, const float* output_membership) {
    float numerator = 0.0f;
    float denominator = 0.0f;
    
    // 定义输出模糊集合的中心点（归一化到[-2, 2]）
    float centers[5] = {-2.0f, -1.0f, 0.0f, 1.0f, 2.0f};
    
    for (int i = 0; i < 5; i++) {
        numerator += output_membership[i] * centers[i];
        denominator += output_membership[i];
    }
    
    if (denominator < 1e-6) {
        return 0.0f;
    }
    
    // 计算归一化的输出
    float normalized_output = numerator / denominator;
    
    // 反归一化到实际范围
    float output = fc->deltaKp_min + 
                   (normalized_output + 2.0f) / 4.0f * 
                   (fc->deltaKp_max - fc->deltaKp_min);
    
    return output;
}

// 模糊PID参数整定
float fuzzy_pid_tuning(FuzzyController* fc, float e, float ec) {
    // 模糊化输入
    float e_membership[5];
    float ec_membership[5];
    
    fuzzify_input(e, fc->e_min, fc->e_max, e_membership);
    fuzzify_input(ec, fc->ec_min, fc->ec_max, ec_membership);
    
    // 模糊推理
    float output_membership[5];
    fuzzy_inference(fc, e_membership, ec_membership, output_membership);
    
    // 去模糊化得到ΔKp
    float deltaKp = defuzzify(fc, output_membership);
    
    return deltaKp;
}

// 模拟温度传感器读数
float read_temperature_sensor(void) {
    // 模拟传感器读数，返回一个随机温度值
    static int count = 0;
    float base_temp = 25.0f;
    float variations[] = {0.5f, -0.3f, 0.8f, -0.6f, 0.2f};
    return base_temp + variations[count++ % 5];
}

// 模拟PWM输出
void set_pwm_output(float duty_cycle) {
    printf("[模拟PWM] 设置占空比: %.2f%%\n", duty_cycle * 100.0f);
}

// 打印PID参数
void print_pid_params(const PIDParams* pid) {
    printf("当前PID参数: Kp=%.4f, Ki=%.4f, Kd=%.4f\n", 
           pid->Kp, pid->Ki, pid->Kd);
}

int main() {
    printf("=== PID模糊自整定算法测试 ===\n\n");
    
    // 初始化PID参数
    PIDParams pid = {1.0f, 0.1f, 0.05f};
    
    // 初始化模糊控制器
    FuzzyController fc;
    init_fuzzy_controller(&fc);
    
    // 测试用例
    float test_cases[][2] = {
        {10.0f, 2.0f},   // 误差正大，误差变化率正小
        {-5.0f, -1.0f},  // 误差负小，误差变化率负小
        {0.5f, 0.1f}     // 误差接近零，误差变化率接近零
    };
    
    // 模拟3组测试
    for (int i = 0; i < 3; i++) {
        printf("--- 测试用例 %d ---\n", i + 1);
        printf("输入: e=%.2f°C, ec=%.2f°C/s\n", 
               test_cases[i][0], test_cases[i][1]);
        
        // 模糊整定得到ΔKp
        float deltaKp = fuzzy_pid_tuning(&fc, test_cases[i][0], test_cases[i][1]);
        printf("模糊整定结果: ΔKp=%.4f\n", deltaKp);
        
        // 更新PID参数
        pid.Kp += deltaKp;
        // Ki和Kd可以根据需要按比例调整，这里简化处理
        pid.Ki = pid.Kp * 0.1f;
        pid.Kd = pid.Kp * 0.05f;
        
        // 打印调整后的参数
        print_pid_params(&pid);
        
        // 模拟温度控制过程
        printf("\n模拟温度控制过程:\n");
        float setpoint = 30.0f;
        float current_temp = read_temperature_sensor();
        float error = setpoint - current_temp;
        printf("  设定温度: %.1f°C\n", setpoint);
        printf("  当前温度: %.1f°C\n", current_temp);
        printf("  温度误差: %.2f°C\n", error);
        
        // 简单PID计算（仅用于演示）
        float control_output = pid.Kp * error;
        printf("  控制输出: %.2f\n", control_output);
        
        // 模拟PWM输出
        float duty_cycle = fmaxf(0.0f, fminf(1.0f, control_output / 100.0f));
        set_pwm_output(duty_cycle);
        
        printf("\n");
    }
    
    printf("=== 测试完成 ===\n");
    
    return 0;
}