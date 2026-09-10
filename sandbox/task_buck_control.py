import random
import time

# Buck converter parameters
VIN = 12.0
VREF = 5.0
TS = 0.001

# PI controller parameters
KP = 0.1
KI = 0.05

class PI_Controller:
    def __init__(self, kp, ki, min_output, max_output):
        self.kp = kp
        self.ki = ki
        self.integral = 0.0
        self.prev_error = 0.0
        self.output_min = min_output
        self.output_max = max_output
    
    def update(self, error):
        # Proportional term
        proportional = self.kp * error
        
        # Integral term with anti-windup
        self.integral += self.ki * error * TS
        
        # Calculate output
        output = proportional + self.integral
        
        # Clamp output to prevent windup
        if output > self.output_max:
            output = self.output_max
            self.integral = self.output_max - proportional
        elif output < self.output_min:
            output = self.output_min
            self.integral = self.output_min - proportional
        
        self.prev_error = error
        return output

def simulate_adc(duty_cycle):
    # Ideal Vout = Vin * D
    ideal_vout = VIN * duty_cycle
    
    # Add random noise [-0.1V, 0.1V]
    noise = random.uniform(-0.1, 0.1)
    vout = ideal_vout + noise
    
    # Convert to ADC value (voltage * 1000)
    adc_value = int(vout * 1000.0)
    
    # Clamp to reasonable range
    if adc_value < 0:
        adc_value = 0
    if adc_value > 12000:
        adc_value = 12000
    
    return adc_value

def adc_to_voltage(adc_value):
    return adc_value / 1000.0

def main():
    # Seed random number generator
    random.seed(time.time())
    
    # Initialize PI controller
    pi = PI_Controller(KP, KI, 0.05, 0.95)
    
    # Initial duty cycle
    duty_cycle = 0.5
    
    print("Buck Converter Digital Control Simulation")
    print(f"Vin = {VIN:.1f}V, Vref = {VREF:.1f}V, Ts = {TS:.3f}s")
    print(f"PI: Kp = {KP:.2f}, Ki = {KI:.2f}")
    print(f"Duty cycle range: 0.05 - 0.95\n")
    
    print("Cycle\tADC Value\tVout (V)\tError (V)\tDuty Cycle")
    print("----------------------------------------------------------------")
    
    # Simulate 5 control cycles
    for cycle in range(1, 6):
        # Simulate ADC reading
        adc_value = simulate_adc(duty_cycle)
        vout = adc_to_voltage(adc_value)
        
        # Calculate error
        error = VREF - vout
        
        # Update PI controller to get new duty cycle
        new_duty_cycle = pi.update(error)
        
        # Clamp duty cycle to safe range
        if new_duty_cycle > 0.95:
            new_duty_cycle = 0.95
        if new_duty_cycle < 0.05:
            new_duty_cycle = 0.05
        
        # Update duty cycle for next cycle
        duty_cycle = new_duty_cycle
        
        # Print results
        print(f"{cycle}\t{adc_value}\t\t{vout:.3f}\t\t{error:.3f}\t\t{duty_cycle:.3f}")
    
    print("\nSimulation completed.")

if __name__ == "__main__":
    main()