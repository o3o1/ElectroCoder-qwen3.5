class BuckConverter:
    def __init__(self):
        self.Vin = 12.0
        self.Vout = 0.0
        self.Vref = 5.0
        self.D = 0.0
        self.Kp = 0.1
        self.Ki = 2.0
        self.integral_sum = 0.0
        self.Ts = 0.001
    
    def update(self):
        error = self.Vref - self.Vout
        self.integral_sum += error * self.Ts
        D_new = self.Kp * error + self.Ki * self.integral_sum
        if D_new < 0.0:
            D_new = 0.0
        if D_new > 1.0:
            D_new = 1.0
        self.D = D_new
        self.Vout = self.D * self.Vin

def main():
    conv = BuckConverter()
    for i in range(10):
        conv.update()
        print(f"Cycle {i}: D={conv.D:.3f}, Vout={conv.Vout:.3f}")

if __name__ == "__main__":
    main()