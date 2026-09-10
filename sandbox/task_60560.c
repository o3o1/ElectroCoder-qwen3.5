#include <stdio.h>
#include <math.h>

#define SQRT3 1.73205080757f
#define COS120 -0.5f
#define SIN120 0.86602540378f

void calculate_negative_sequence(float ua, float ub, float uc, float *alpha, float *beta) {
    float alpha_val = ua;
    float beta_val = (ub - uc) / SQRT3;
    
    float alpha_cos120 = alpha_val * COS120;
    float beta_sin120 = beta_val * SIN120;
    float alpha_sin120 = alpha_val * SIN120;
    float beta_cos120 = beta_val * COS120;
    
    *alpha = (alpha_cos120 + beta_sin120) / 3.0f;
    *beta = (-alpha_sin120 + beta_cos120) / 3.0f;
}

void compensation(float *mod_a, float *mod_b, float *mod_c, float alpha_neg, float beta_neg) {
    float ua_comp = alpha_neg;
    float ub_comp = -0.5f * alpha_neg + (SQRT3/2.0f) * beta_neg;
    float uc_comp = -0.5f * alpha_neg - (SQRT3/2.0f) * beta_neg;
    
    *mod_a = *mod_a - ua_comp;
    *mod_b = *mod_b - ub_comp;
    *mod_c = *mod_c - uc_comp;
}

int main() {
    float ua = 1.0f;
    float ub = -0.5f;
    float uc = -0.5f;
    
    float alpha_neg, beta_neg;
    calculate_negative_sequence(ua, ub, uc, &alpha_neg, &beta_neg);
    printf("alpha_neg=%.3f, beta_neg=%.3f\n", alpha_neg, beta_neg);
    
    float mod_a = 0.8f;
    float mod_b = 0.4f;
    float mod_c = 0.4f;
    
    compensation(&mod_a, &mod_b, &mod_c, alpha_neg, beta_neg);
    printf("mod_a=%.3f, mod_b=%.3f, mod_c=%.3f\n", mod_a, mod_b, mod_c);
    
    return 0;
}