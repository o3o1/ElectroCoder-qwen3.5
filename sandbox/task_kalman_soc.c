#include <stdio.h>
#include <math.h>

typedef struct {
    float soc_est;  // SOC estimate (0-1)
    float P;        // Estimation error covariance
    float Q;        // Process noise covariance
    float R;        // Measurement noise covariance
    float K;        // Kalman gain
} KalmanFilter;

float voltage_to_soc(float voltage) {
    // Simplified linear relationship: 3.6V=0%, 4.2V=100%
    float soc = (voltage - 3.6f) / 0.6f;
    return soc < 0.0f ? 0.0f : (soc > 1.0f ? 1.0f : soc);
}

void kalman_init(KalmanFilter* kf, float init_soc, float init_P, float Q, float R) {
    kf->soc_est = init_soc;
    kf->P = init_P;
    kf->Q = Q;
    kf->R = R;
    kf->K = 0.0f;
}

float kalman_update(KalmanFilter* kf, float current, float voltage, float dt) {
    const float capacity = 10.0f;  // 10Ah
    
    // Prediction step
    float soc_pred = kf->soc_est - (current * dt) / (capacity * 3600.0f);  // Ah to As
    float P_pred = kf->P + kf->Q;
    
    // Update step
    float soc_meas = voltage_to_soc(voltage);
    kf->K = P_pred / (P_pred + kf->R);
    kf->soc_est = soc_pred + kf->K * (soc_meas - soc_pred);
    kf->P = (1.0f - kf->K) * P_pred;
    
    return kf->soc_est;
}

int main() {
    KalmanFilter kf;
    
    // Initialize filter
    kalman_init(&kf, 0.5f, 0.1f, 0.01f, 0.1f);
    
    // Test data
    float currents[] = {5.0f, 4.8f, 5.1f};
    float voltages[] = {3.7f, 3.68f, 3.72f};
    float dt = 0.1f;
    
    printf("Kalman Filter SOC Estimation Test\n");
    printf("==================================\n");
    
    for (int i = 0; i < 3; i++) {
        float soc = kalman_update(&kf, currents[i], voltages[i], dt);
        printf("Step %d:\n", i+1);
        printf("  Current: %.2f A, Voltage: %.3f V\n", currents[i], voltages[i]);
        printf("  Measured SOC: %.4f\n", voltage_to_soc(voltages[i]));
        printf("  Estimated SOC: %.4f\n", soc);
        printf("  Kalman Gain: %.4f\n", kf.K);
        printf("  Error Covariance P: %.4f\n", kf.P);
        printf("\n");
    }
    
    return 0;
}