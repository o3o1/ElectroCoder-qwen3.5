import math

def dead_time_compensation(duty_cycle, dead_time_us, switching_period_us, polarity):
    compensation = duty_cycle + (polarity * dead_time_us / switching_period_us)
    
    if compensation < 0.0:
        return 0.0
    if compensation > 1.0:
        return 1.0
    return compensation

print('Dead-time Compensation Algorithm Test')
print('=====================================')
print()

# Test case a
duty_a = 0.3
dead_time_a = 2.0
period_a = 100.0
polarity_a = 1
comp_a = dead_time_compensation(duty_a, dead_time_a, period_a, polarity_a)
print('Test case a:')
print('  Original duty cycle: {:.3f}'.format(duty_a))
print('  Current polarity: {}'.format('Positive(+1)' if polarity_a > 0 else 'Negative(-1)'))
print('  Dead time: {:.1f} us'.format(dead_time_a))
print('  Switching period: {:.1f} us'.format(period_a))
print('  Compensated duty cycle: {:.3f}'.format(comp_a))
print()

# Test case b
duty_b = 0.8
dead_time_b = 2.0
period_b = 100.0
polarity_b = -1
comp_b = dead_time_compensation(duty_b, dead_time_b, period_b, polarity_b)
print('Test case b:')
print('  Original duty cycle: {:.3f}'.format(duty_b))
print('  Current polarity: {}'.format('Positive(+1)' if polarity_b > 0 else 'Negative(-1)'))
print('  Dead time: {:.1f} us'.format(dead_time_b))
print('  Switching period: {:.1f} us'.format(period_b))
print('  Compensated duty cycle: {:.3f}'.format(comp_b))
print()

# Test case c
duty_c = 0.05
dead_time_c = 3.0
period_c = 50.0
polarity_c = 1
comp_c = dead_time_compensation(duty_c, dead_time_c, period_c, polarity_c)
print('Test case c:')
print('  Original duty cycle: {:.3f}'.format(duty_c))
print('  Current polarity: {}'.format('Positive(+1)' if polarity_c > 0 else 'Negative(-1)'))
print('  Dead time: {:.1f} us'.format(dead_time_c))
print('  Switching period: {:.1f} us'.format(period_c))
print('  Compensated duty cycle: {:.3f}'.format(comp_c))
print()

# Test case d
duty_d = 0.95
dead_time_d = 3.0
period_d = 50.0
polarity_d = -1
comp_d = dead_time_compensation(duty_d, dead_time_d, period_d, polarity_d)
print('Test case d:')
print('  Original duty cycle: {:.3f}'.format(duty_d))
print('  Current polarity: {}'.format('Positive(+1)' if polarity_d > 0 else 'Negative(-1)'))
print('  Dead time: {:.1f} us'.format(dead_time_d))
print('  Switching period: {:.1f} us'.format(period_d))
print('  Compensated duty cycle: {:.3f}'.format(comp_d))