#include <stdio.h>
#include <math.h>

#define VDC 300.0
#define R 10.0
#define L 0.01
#define TS 0.0001

typedef struct {
    double v_alpha;
    double v_beta;
} VoltageVector;

typedef struct {
    int sa;
    int sb;
    int sc;
} SwitchState;

void precompute_voltage_vectors(VoltageVector vectors[8]) {
    const double two_thirds = 2.0 / 3.0;
    const double sqrt3_div3 = sqrt(3.0) / 3.0;
    
    for (int i = 0; i < 8; i++) {
        int sa = (i >> 2) & 1;
        int sb = (i >> 1) & 1;
        int sc = i & 1;
        
        vectors[i].v_alpha = two_thirds * VDC * (sa - 0.5 * (sb + sc));
        vectors[i].v_beta = sqrt3_div3 * VDC * (sb - sc);
    }
}

void predict_current(double i_alpha_k, double i_beta_k, double v_alpha, double v_beta,
                     double* i_alpha_k1, double* i_beta_k1) {
    double factor = 1.0 - R * TS / L;
    double gain = TS / L;
    
    *i_alpha_k1 = factor * i_alpha_k + gain * v_alpha;
    *i_beta_k1 = factor * i_beta_k + gain * v_beta;
}

double compute_cost(double i_alpha_ref, double i_beta_ref, 
                    double i_alpha_k1, double i_beta_k1) {
    double error_alpha = i_alpha_ref - i_alpha_k1;
    double error_beta = i_beta_ref - i_beta_k1;
    return error_alpha * error_alpha + error_beta * error_beta;
}

void find_optimal_switch_state(double i_alpha_k, double i_beta_k,
                               double i_alpha_ref, double i_beta_ref,
                               VoltageVector vectors[8],
                               int* optimal_state, double* min_cost) {
    *min_cost = 1e20;
    *optimal_state = 0;
    
    for (int i = 0; i < 8; i++) {
        double i_alpha_k1, i_beta_k1;
        predict_current(i_alpha_k, i_beta_k, 
                       vectors[i].v_alpha, vectors[i].v_beta,
                       &i_alpha_k1, &i_beta_k1);
        
        double cost = compute_cost(i_alpha_ref, i_beta_ref, i_alpha_k1, i_beta_k1);
        
        if (cost < *min_cost) {
            *min_cost = cost;
            *optimal_state = i;
        }
    }
}

void print_switch_state(int state) {
    int sa = (state >> 2) & 1;
    int sb = (state >> 1) & 1;
    int sc = state & 1;
    printf("%d%d%d", sa, sb, sc);
}

int main() {
    VoltageVector vectors[8];
    precompute_voltage_vectors(vectors);
    
    printf("逆变器MPC单步成本函数优化算法\n");
    printf("==============================\n\n");
    
    printf("测试1:\n");
    printf("当前电流: iα=0.5A, iβ=0.2A\n");
    printf("参考电流: iα_ref=1.0A, iβ_ref=0.5A\n");
    
    int optimal_state1;
    double min_cost1;
    find_optimal_switch_state(0.5, 0.2, 1.0, 0.5, vectors, &optimal_state1, &min_cost1);
    
    printf("最优开关状态: ");
    print_switch_state(optimal_state1);
    printf("\n最小成本值: %.6f\n\n", min_cost1);
    
    printf("测试2:\n");
    printf("当前电流: iα=-0.3A, iβ=0.8A\n");
    printf("参考电流: iα_ref=0.2A, iβ_ref=1.0A\n");
    
    int optimal_state2;
    double min_cost2;
    find_optimal_switch_state(-0.3, 0.8, 0.2, 1.0, vectors, &optimal_state2, &min_cost2);
    
    printf("最优开关状态: ");
    print_switch_state(optimal_state2);
    printf("\n最小成本值: %.6f\n", min_cost2);
    
    return 0;
}