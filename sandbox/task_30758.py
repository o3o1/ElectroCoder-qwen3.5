import math

def calculate_THD(fundamental_amplitude, harmonics):
    sum_squares = 0.0
    
    for harmonic in harmonics:
        sum_squares += harmonic * harmonic
    
    if fundamental_amplitude == 0.0:
        return 0.0
    
    return math.sqrt(sum_squares) / fundamental_amplitude * 100.0

# Test case 1
harmonics1 = [1.2, 0.8, 0.3]
thd1 = calculate_THD(10.0, harmonics1)
print(f"Test case 1: THD = {thd1:.2f}%")

# Test case 2
harmonics2 = [0.5, 0.2]
thd2 = calculate_THD(5.0, harmonics2)
print(f"Test case 2: THD = {thd2:.2f}%")

# Test case 3
harmonics3 = []
thd3 = calculate_THD(8.0, harmonics3)
print(f"Test case 3: THD = {thd3:.2f}%")