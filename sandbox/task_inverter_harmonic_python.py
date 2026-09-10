import math

PI = 3.14159265358979323846

class RepetitiveController:
    def __init__(self):
        self.write_idx = 0
        self.prev_output = 0.0
        
    def control(self, error_buffer, buffer_len, k_r):
        current_error = error_buffer[self.write_idx]
        new_output = k_r * (self.prev_output + current_error) + (1 - k_r) * self.prev_output
        
        self.prev_output = new_output
        self.write_idx = (self.write_idx + 1) % buffer_len
        
        return new_output

def main():
    fs = 10000.0
    f0 = 50.0
    buffer_len = int(fs / f0)
    
    error_buffer = [0.0] * buffer_len
    Ts = 1.0 / fs
    
    controller = RepetitiveController()
    write_idx = 0
    
    for cycle in range(5):
        max_output = -1e9
        min_output = 1e9
        
        for i in range(buffer_len):
            t = (cycle * buffer_len + i) * Ts
            error = 0.1 * math.sin(2 * PI * 50 * t) + 0.05 * math.sin(2 * PI * 150 * t)
            
            error_buffer[write_idx] = error
            output = controller.control(error_buffer, buffer_len, 0.5)
            
            if output > max_output:
                max_output = output
            if output < min_output:
                min_output = output
            
            write_idx = (write_idx + 1) % buffer_len
        
        print(f'Cycle {cycle + 1}: Max output = {max_output:.6f}, Min output = {min_output:.6f}')

if __name__ == "__main__":
    main()