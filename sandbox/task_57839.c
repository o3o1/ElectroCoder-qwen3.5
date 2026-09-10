#include <stdio.h>
#include <math.h>

typedef struct {
    double kp, ki, kd;
    double integral;
    double prev_error;
    double dead_zone;
    double output_limit;
} DeadZonePID;

double deadzone_pid_update(DeadZonePID *pid, double setpoint, double measurement, double dt) {
    double error = setpoint - measurement;
    double derivative;
    double output;
    
    if (fabs(error) <= pid->dead_zone) {
        // 死区内，跳过积分
    } else {
        // 死区外，正常积分
        pid->integral += error * dt;
    }
    
    derivative = (error - pid->prev_error) / dt;
    output = pid->kp * error + pid->ki * pid->integral + pid->kd * derivative;
    
    // 输出限幅
    if (fabs(output) > pid->output_limit) {
        output = (output > 0) ? pid->output_limit : -pid->output_limit;
    }
    
    pid->prev_error = error;
    return output;
}

int main() {
    DeadZonePID pid = {
        .kp = 1.5,
        .ki = 0.8,
        .kd = 0.2,
        .integral = 0.0,
        .prev_error = 0.0,
        .dead_zone = 0.05,
        .output_limit = 10.0
    };
    
    double dt = 0.1;
    double setpoints[] = {5.0, 5.0, 5.0, 5.0, 5.0};
    double measurements[] = {4.8, 5.02, 4.97, 5.06, 4.93};
    
    printf("周期序号\t设定值\t测量值\t计算误差\t控制器输出\n");
    printf("------------------------------------------------------------\n");
    
    for (int i = 0; i < 5; i++) {
        double output = deadzone_pid_update(&pid, setpoints[i], measurements[i], dt);
        double error = setpoints[i] - measurements[i];
        
        printf("%d\t\t%.2f\t%.2f\t%.3f\t\t%.3f\n", 
               i+1, setpoints[i], measurements[i], error, output);
    }
    
    return 0;
}