import math

SQRT3 = math.sqrt(3)

def determine_sector(Ualpha, Ubeta):
    A = Ubeta
    B = 0.5 * (SQRT3 * Ualpha - Ubeta)
    C = -0.5 * (SQRT3 * Ualpha + Ubeta)
    
    if A > 0 and C <= 0: return 1
    if A <= 0 and B > 0: return 2
    if B <= 0 and C > 0: return 3
    if A <= 0 and C > 0: return 4
    if A > 0 and B <= 0: return 5
    if B > 0 and C <= 0: return 6
    
    return 1

test_cases = [
    (1.0, 0.0),
    (0.5, 0.866),
    (-0.5, -0.5),
    (0.0, 1.0),
    (-1.0, 0.0)
]

print('SVPWM sector test:')
print('==================')
for i, (Ualpha, Ubeta) in enumerate(test_cases):
    sector = determine_sector(Ualpha, Ubeta)
    print(f'Test case {i+1}: Ualpha={Ualpha:.3f}, Ubeta={Ubeta:.3f} -> Sector {sector}')