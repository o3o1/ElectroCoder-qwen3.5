#!/usr/bin/env python3
# Boost Converter Digital Control Simulation

class PI_Controller:
    def __init__(self, kp, ki, output_min, output_max):
        self.kp = kp
        self.ki = ki
        self.integral = 0.0
        self.output_min = output_min
        self.output_max = output_max
    
    def update(self, error, dt):
        # Proportional term
        proportional = self.kp * error
        
        # Integral term with anti-windup
        self.integral += self.ki * error * dt
        
        # Calculate total output
        output = proportional + self.integral
        
        # Limit output with anti-windup
        if output > self.output_max:
            output = self.output_max
            self.integral = self.output_max - proportional
        elif output < self.output_min:
            output = self.output_min
            self.integral = self.output_min - proportional
        
        return output

def limit_duty(duty):
    """Limit duty cycle to 0.1-0.9 range"""
    if duty < 0.1:
        return 0.1
    if duty > 0.9:
        return 0.9
    return duty

def main():
    # Initialize PI controller
    pi = PI_Controller(kp=0.05, ki=0.01, output_min=-0.5, output_max=0.5)
    
    target_voltage = 24.0  # Target output voltage
    dt = 0.01  # Control period in seconds (10ms)
    
    # Simulated ADC readings (starting below target and converging)
    sampled_voltages = [23.1, 23.8, 24.2, 24.0, 24.1]
    
    print("Boost Converter Digital Control Simulation")
    print(f"Target Voltage: {target_voltage:.1f}V")
    print()
    
    # Simulate 5 control cycles
    for cycle in range(5):
        # 1. Read ADC (simulate voltage measurement)
        sampled_voltage = sampled_voltages[cycle]
        
        # 2. Calculate error
        error = target_voltage - sampled_voltage
        
        # 3. Update PI controller
        pi_output = pi.update(error, dt)
        
        # 4. Calculate duty cycle (base duty + PI correction)
        # For Boost converter: Vout = Vin/(1-D), so D = 1 - Vin/Vout
        # With Vin=12V, Vout=24V, ideal D = 0.5
        base_duty = 0.5  # Ideal duty for 12V->24V conversion
        duty_cycle = base_duty + pi_output
        
        # 5. Limit duty cycle
        duty_cycle = limit_duty(duty_cycle)
        
        # 6. Print results
        print(f"周期{cycle+1}: 采样={sampled_voltage:.1f}V, 误差={error:.1f}V, PI输出={pi_output:.2f}, 占空比={duty_cycle:.2f}")

if __name__ == "__main__":
    main()