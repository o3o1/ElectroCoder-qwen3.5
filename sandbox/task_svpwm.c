#include <stdio.h>
#include <math.h>

#define SQRT3 1.73205080757

typedef struct {
    float Va_ref, Vb_ref, Vc_ref;  // 输入
    int sector;                    // 输出：扇区号0-5
    float V_alpha, V_beta;         // 输出：α-β分量
    float T1, T2;                  // 输出：基本矢量作用时间
    float Ta, Tb, Tc;              // 输出：PWM比较值
} SVPWM_Result;

void svpwm_calculate(float Va, float Vb, float Vc, SVPWM_Result *result) {
    // 保存输入值
    result->Va_ref = Va;
    result->Vb_ref = Vb;
    result->Vc_ref = Vc;
    
    // 1. 计算V_alpha, V_beta
    result->V_alpha = Va;
    result->V_beta = (Vb - Vc) / SQRT3;
    
    // 2. 判断扇区
    float X = result->V_beta;
    float Y = SQRT3 * result->V_alpha - result->V_beta;
    float Z = -SQRT3 * result->V_alpha - result->V_beta;
    
    int A = (X > 0) ? 1 : 0;
    int B = (Y > 0) ? 1 : 0;
    int C = (Z > 0) ? 1 : 0;
    
    int sector = A + 2*B + 4*C;
    
    switch(sector) {
        case 3: result->sector = 0; break;
        case 1: result->sector = 1; break;
        case 5: result->sector = 2; break;
        case 4: result->sector = 3; break;
        case 6: result->sector = 4; break;
        case 2: result->sector = 5; break;
        default: result->sector = 0; break;
    }
    
    // 3. 计算T1, T2
    float T = 1.0f;  // 标幺化周期
    float Vdc = 1.0f;  // 标幺化直流母线电压
    
    float Vref_alpha = result->V_alpha;
    float Vref_beta = result->V_beta;
    
    // 计算调制系数
    float m = sqrtf(Vref_alpha*Vref_alpha + Vref_beta*Vref_beta) * 2.0f / Vdc;
    
    // 根据扇区计算T1, T2
    switch(result->sector) {
        case 0:
            result->T1 = m * (SQRT3 * Vref_alpha - Vref_beta) / 2.0f;
            result->T2 = m * Vref_beta;
            break;
        case 1:
            result->T1 = m * (SQRT3 * Vref_alpha + Vref_beta) / 2.0f;
            result->T2 = m * (-SQRT3 * Vref_alpha + Vref_beta) / 2.0f;
            break;
        case 2:
            result->T1 = m * Vref_beta;
            result->T2 = m * (-SQRT3 * Vref_alpha - Vref_beta) / 2.0f;
            break;
        case 3:
            result->T1 = m * (-Vref_beta);
            result->T2 = m * (-SQRT3 * Vref_alpha + Vref_beta) / 2.0f;
            break;
        case 4:
            result->T1 = m * (-SQRT3 * Vref_alpha - Vref_beta) / 2.0f;
            result->T2 = m * (SQRT3 * Vref_alpha - Vref_beta) / 2.0f;
            break;
        case 5:
            result->T1 = m * (-SQRT3 * Vref_alpha + Vref_beta) / 2.0f;
            result->T2 = m * Vref_beta;
            break;
        default:
            result->T1 = 0;
            result->T2 = 0;
            break;
    }
    
    // 限制T1, T2在[0, 1]范围内
    if (result->T1 < 0) result->T1 = 0;
    if (result->T1 > 1) result->T1 = 1;
    if (result->T2 < 0) result->T2 = 0;
    if (result->T2 > 1) result->T2 = 1;
    
    // 4. 计算Ta, Tb, Tc
    float T0 = T - result->T1 - result->T2;
    if (T0 < 0) T0 = 0;
    
    float T1 = result->T1;
    float T2 = result->T2;
    
    switch(result->sector) {
        case 0:
            result->Ta = T1 + T2 + T0/2;
            result->Tb = T2 + T0/2;
            result->Tc = T0/2;
            break;
        case 1:
            result->Ta = T1 + T0/2;
            result->Tb = T1 + T2 + T0/2;
            result->Tc = T0/2;
            break;
        case 2:
            result->Ta = T0/2;
            result->Tb = T1 + T2 + T0/2;
            result->Tc = T2 + T0/2;
            break;
        case 3:
            result->Ta = T0/2;
            result->Tb = T1 + T0/2;
            result->Tc = T1 + T2 + T0/2;
            break;
        case 4:
            result->Ta = T2 + T0/2;
            result->Tb = T0/2;
            result->Tc = T1 + T2 + T0/2;
            break;
        case 5:
            result->Ta = T1 + T2 + T0/2;
            result->Tb = T0/2;
            result->Tc = T1 + T0/2;
            break;
        default:
            result->Ta = 0.5f;
            result->Tb = 0.5f;
            result->Tc = 0.5f;
            break;
    }
}

int main() {
    // 测试用例1：扇区0
    SVPWM_Result test1;
    svpwm_calculate(0.5f, -0.2f, -0.3f, &test1);
    printf("Test1 - Va=0.5, Vb=-0.2, Vc=-0.3:\n");
    printf("  Sector=%d, Vα=%.3f, Vβ=%.3f\n", test1.sector, test1.V_alpha, test1.V_beta);
    printf("  T1=%.3f, T2=%.3f, Ta=%.3f, Tb=%.3f, Tc=%.3f\n\n", 
           test1.T1, test1.T2, test1.Ta, test1.Tb, test1.Tc);
    
    // 测试用例2：扇区2
    SVPWM_Result test2;
    svpwm_calculate(-0.3f, 0.6f, -0.3f, &test2);
    printf("Test2 - Va=-0.3, Vb=0.6, Vc=-0.3:\n");
    printf("  Sector=%d, Vα=%.3f, Vβ=%.3f\n", test2.sector, test2.V_alpha, test2.V_beta);
    printf("  T1=%.3f, T2=%.3f, Ta=%.3f, Tb=%.3f, Tc=%.3f\n\n", 
           test2.T1, test2.T2, test2.Ta, test2.Tb, test2.Tc);
    
    // 测试用例3：扇区4
    SVPWM_Result test3;
    svpwm_calculate(-0.4f, -0.1f, 0.5f, &test3);
    printf("Test3 - Va=-0.4, Vb=-0.1, Vc=0.5:\n");
    printf("  Sector=%d, Vα=%.3f, Vβ=%.3f\n", test3.sector, test3.V_alpha, test3.V_beta);
    printf("  T1=%.3f, T2=%.3f, Ta=%.3f, Tb=%.3f, Tc=%.3f\n\n", 
           test3.T1, test3.T2, test3.Ta, test3.Tb, test3.Tc);
    
    return 0;
}