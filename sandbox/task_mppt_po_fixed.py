import sys

class PVPanel:
    def __init__(self):
        self.voltage = 0.0
        self.current = 0.0
        self.power = 0.0
        self.ref_voltage = 10.0
    
    def measure_pv_parameters(self):
        self.voltage = self.ref_voltage
        if self.voltage < 0.0:
            self.voltage = 0.0
        if self.voltage > 50.0:
            self.voltage = 50.0
        
        self.current = 5.0 - 0.1 * self.voltage
        if self.current < 0.0:
            self.current = 0.0
        
        self.power = self.voltage * self.current
    
    def perturb_and_observe(self, delta_v):
        old_power = self.power
        old_ref_voltage = self.ref_voltage
        
        self.ref_voltage += delta_v
        if self.ref_voltage < 0.0:
            self.ref_voltage = 0.0
        if self.ref_voltage > 50.0:
            self.ref_voltage = 50.0
        
        self.measure_pv_parameters()
        
        if self.power < old_power:
            delta_v = -delta_v
            self.ref_voltage = old_ref_voltage + delta_v
            if self.ref_voltage < 0.0:
                self.ref_voltage = 0.0
            if self.ref_voltage > 50.0:
                self.ref_voltage = 50.0
            self.measure_pv_parameters()

def main():
    panel = PVPanel()
    delta_v = 0.5
    
    panel.measure_pv_parameters()
    
    print("Iteration\tRef Voltage(V)\tOutput Voltage(V)\tOutput Current(A)\tPower(W)")
    print(f"0\t\t{panel.ref_voltage:.2f}\t\t{panel.voltage:.2f}\t\t{panel.current:.2f}\t\t{panel.power:.2f}")
    
    max_power = panel.power
    max_power_voltage = panel.voltage
    
    for i in range(1, 11):
        panel.perturb_and_observe(delta_v)
        
        print(f"{i}\t\t{panel.ref_voltage:.2f}\t\t{panel.voltage:.2f}\t\t{panel.current:.2f}\t\t{panel.power:.2f}")
        
        if panel.power > max_power:
            max_power = panel.power
            max_power_voltage = panel.voltage
    
    print("\nFinal Maximum Power Point:")
    print(f"Voltage: {max_power_voltage:.2f} V")
    print(f"Power: {max_power:.2f} W")

if __name__ == "__main__":
    main()