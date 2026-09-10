import sys

class BuckPIController:
    def __init__(self, kp, ki, integral_limit, delay_cycles):
        self.kp = kp
        self.ki = ki
        self.integral = 0.0
        self.integral_limit = integral_limit
        self.compensation_delay = delay_cycles
        
        if delay_cycles > 0:
            self.compensation_buffer = [0.0] * delay_cycles
        else:
            self.compensation_buffer = None
            
        self.buffer_index = 0
    
    def update(self, v_ref, v_fb, dt):
        error = v_ref - v_fb
        
        self.integral += self.ki * error * dt
        
        if self.integral > self.integral_limit:
            self.integral = self.integral_limit
        elif self.integral < -self.integral_limit:
            self.integral = -self.integral_limit
        
        proportional = self.kp * error
        output = proportional + self.integral
        
        if self.compensation_delay > 0:
            self.compensation_buffer[self.buffer_index] = output
            self.buffer_index = (self.buffer_index + 1) % self.compensation_delay
            
            delayed_index = (self.buffer_index + self.compensation_delay - 1) % self.compensation_delay
            return self.compensation_buffer[delayed_index]
        
        return output
    
    def cleanup(self):
        self.compensation_buffer = None

def main():
    ctrl = BuckPIController(0.5, 10.0, 1.0, 3)
    
    dt = 0.001
    v_ref_values = [5.0, 5.0, 5.0, 5.0, 5.0]
    v_fb_values = [4.5, 4.8, 4.9, 5.0, 5.1]
    
    print("Buck变换器电压闭环PI控制仿真")
    print(f"参数: kp={ctrl.kp:.1f}, ki={ctrl.ki:.1f}, 积分限幅={ctrl.integral_limit:.1f}, 延迟补偿={ctrl.compensation_delay}周期\n")
    print("周期\tv_ref\tv_fb\t误差\t\t积分值\t\t补偿前输出\t补偿后输出")
    print("-----------------------------------------------------------------------------")
    
    for i in range(5):
        v_ref = v_ref_values[i]
        v_fb = v_fb_values[i]
        error = v_ref - v_fb
        integral_before = ctrl.integral
        
        output_before = ctrl.kp * error + integral_before
        output_compensated = ctrl.update(v_ref, v_fb, dt)
        
        print(f"{i+1}\t{v_ref:.1f}\t{v_fb:.1f}\t{error:.3f}\t\t{ctrl.integral:.6f}\t{output_before:.6f}\t{output_compensated:.6f}")
    
    ctrl.cleanup()

if __name__ == "__main__":
    main()