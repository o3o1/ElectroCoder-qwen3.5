import math

PI = 3.1415926535
RATED_AMPLITUDE = 311.0
SAG_THRESHOLD = 0.8

def detect_voltage_sag(voltages):
    sum_abs = sum(abs(v) for v in voltages)
    avg_amplitude = sum_abs / 3.0
    return avg_amplitude < (RATED_AMPLITUDE * SAG_THRESHOLD)

def compensate_amplitude(phase_angle):
    compensated = [0.0, 0.0, 0.0]
    compensated[0] = RATED_AMPLITUDE * math.sin(phase_angle)
    compensated[1] = RATED_AMPLITUDE * math.sin(phase_angle - 2.0 * PI / 3.0)
    compensated[2] = RATED_AMPLITUDE * math.sin(phase_angle + 2.0 * PI / 3.0)
    return compensated

print('PLL Voltage Sag Amplitude Locking Algorithm Test (Python模拟)')
print('Rated Amplitude: %.1fV, Sag Threshold: %.0f%%' % (RATED_AMPLITUDE, SAG_THRESHOLD * 100))
print('Sag Detection Threshold: %.1fV' % (RATED_AMPLITUDE * SAG_THRESHOLD))
print()

test_voltages = [
    [300.0, -280.0, 290.0],
    [150.0, -130.0, 140.0],
    [248.0, -250.0, 249.0]
]

test_phases = [0.5, 1.2, 2.0]

for i in range(3):
    print('Test %d:' % (i + 1))
    print('  Input Three-Phase Voltages: [%.1f, %.1f, %.1f] V' % tuple(test_voltages[i]))
    print('  Phase Angle: %.2f radians' % test_phases[i])
    
    is_sag = detect_voltage_sag(test_voltages[i])
    print('  Voltage Sag Detection: %s' % ('Yes' if is_sag else 'No'))
    
    if is_sag:
        compensated = compensate_amplitude(test_phases[i])
        print('  Compensated Three-Phase Voltages: [%.2f, %.2f, %.2f] V' % tuple(compensated))
    print()