import math

PI = 3.141592653589793

def pll_wave_shaper(phase, wave_mode):
    if wave_mode == 0:
        if phase >= 0 and phase < PI:
            return 1.0
        else:
            return -1.0
    else:
        return math.sin(phase)

test_phases = [0.0, PI/4, PI/2, 3*PI/4, PI, 5*PI/4, 3*PI/2, 7*PI/4]

print("=== Square Wave Test ===")
for phase in test_phases:
    output = pll_wave_shaper(phase, 0)
    print(f"Phase: {phase:.2f} rad -> Output: {output:.2f}")

print("\n=== Sine Wave Test ===")
for phase in test_phases:
    output = pll_wave_shaper(phase, 1)
    print(f"Phase: {phase:.2f} rad -> Output: {output:.2f}")