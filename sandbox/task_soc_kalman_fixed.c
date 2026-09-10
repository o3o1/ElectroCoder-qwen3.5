#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define Q_MAX 5000.0  // mAh
#define R_BAT 0.05    // Ohm
#define DT 1.0        // s

typedef struct {
    double x;      // 状态估计值 (SOC)
    double P;      // 误差协方差
    double Q;      // 过程噪声协方差
    double R;      // 观测噪声协方差
} KalmanFilter;

double ocv(double soc) {
    return 3.0 + 0.5 * soc;
}

void kalman_init(KalmanFilter *kf, double init_soc, double init_P, double Q, double R) {
    kf->x = init_soc;
    kf->P = init_P;
    kf->Q = Q;
    kf->R = R;
}

void kalman_predict(KalmanFilter *kf, double I, double dt) {
    double current_ah = I * dt / 3600.0;
    kf->x = kf->x + current_ah / (Q_MAX / 1000.0);
    kf->P = kf->P + kf->Q;
}

void kalman_update(KalmanFilter *kf, double V_meas, double I) {
    double H = 0.5;
    double y = V_meas - (ocv(kf->x) + I * R_BAT);
    double S = kf->P * H * H + kf->R;
    double K = kf->P * H / S;
    
    kf->x = kf->x + K * y;
    kf->P = (1.0 - K * H) * kf->P;
}

double gaussian_noise(double mean, double stddev) {
    double u1 = (rand() + 1.0) / (RAND_MAX + 1.0);
    double u2 = (rand() + 1.0) / (RAND_MAX + 1.0);
    double z = sqrt(-2.0 * log(u1)) * cos(2.0 * 3.141592653589793 * u2);
    return mean + stddev * z;
}

int main() {
    srand(time(NULL));
    
    KalmanFilter kf;
    double init_soc = 0.5;
    double init_P = 0.1;
    double process_noise_std = 0.01;
    double obs_noise_std = 0.05;
    
    kalman_init(&kf, init_soc, init_P, process_noise_std * process_noise_std, 
                obs_noise_std * obs_noise_std);
    
    double true_soc = init_soc;
    double total_error = 0.0;
    
    printf("Time Step | True SOC | Measured V | Estimated SOC\n");
    printf("--------------------------------------------\n");
    
    for (int k = 0; k < 10; k++) {
        double I = -2.0 + 4.0 * rand() / RAND_MAX;
        
        double w_k = gaussian_noise(0.0, process_noise_std);
        double v_k = gaussian_noise(0.0, obs_noise_std);
        
        double current_ah = I * DT / 3600.0;
        true_soc = true_soc + current_ah / (Q_MAX / 1000.0);
        
        if (true_soc > 1.0) true_soc = 1.0;
        if (true_soc < 0.0) true_soc = 0.0;
        
        double V_true = ocv(true_soc) + I * R_BAT;
        double V_meas = V_true + v_k;
        
        kalman_predict(&kf, I, DT);
        kalman_update(&kf, V_meas, I);
        
        if (kf.x > 1.0) kf.x = 1.0;
        if (kf.x < 0.0) kf.x = 0.0;
        
        double error = fabs(true_soc - kf.x);
        total_error += error;
        
        printf("%9d | %8.4f | %10.4f | %12.4f\n", 
               k+1, true_soc, V_meas, kf.x);
    }
    
    printf("--------------------------------------------\n");
    printf("Average estimation error: %.4f\n", total_error / 10.0);
    
    return 0;
}