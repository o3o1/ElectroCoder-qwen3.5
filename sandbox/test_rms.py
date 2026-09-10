import math  
  
def calculate_rms(samples):  
    if not samples or len(samples) == 0:  
        return 0.0  
    sum_squares = sum(x*x for x in samples)  
    return math.sqrt(sum_squares / len(samples))  
  
SAMPLES_PER_CYCLE = 32  
FREQUENCY = 50.0  
SAMPLE_RATE = 1600.0  
PI = 3.141592653589793  
  
samples1 = []  
samples2 = []  
for i in range(SAMPLES_PER_CYCLE):  
    t = i / SAMPLE_RATE  
    samples1.append(100.0 * math.sin(2.0 * PI * FREQUENCY * t))  
    samples2.append(50.0 + 50.0 * math.sin(2.0 * PI * FREQUENCY * t))  
  
rms1 = calculate_rms(samples1)  
rms2 = calculate_rms(samples2)  
  
print('Pure sine wave RMS: {:.4f} A (Theoretical: 70.7107 A)'.format(rms1))  
print('DC offset wave RMS: {:.4f} A (Theoretical: 61.2372 A)'.format(rms2)) 
