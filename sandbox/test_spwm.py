import math

PWM_MAX_VALUE = 1000
SINE_TABLE_SIZE = 256

sine_table = [0.0] * SINE_TABLE_SIZE

def spwm_init():
    for i in range(SINE_TABLE_SIZE):
        angle = 2.0 * math.pi * i / SINE_TABLE_SIZE
        sine_table[i] = (math.sin(angle) + 1.0) / 2.0

def spwm_get_duty_cycle(angle_index, modulation_index):
    sine_value = sine_table[angle_index]
    modulation = modulation_index / 100.0
    duty_ratio = sine_value * modulation
    return int(duty_ratio * PWM_MAX_VALUE)

def main():
    spwm_init()
    
    print("SPWM测试结果：")
    print(f"1. angle_index=0, modulation_index=50 -> duty_cycle={spwm_get_duty_cycle(0, 50)}")
    print(f"2. angle_index=64, modulation_index=75 -> duty_cycle={spwm_get_duty_cycle(64, 75)}")
    print(f"3. angle_index=128, modulation_index=100 -> duty_cycle={spwm_get_duty_cycle(128, 100)}")
    
    print("\n调制比80%时完整周期前10个点：")
    for i in range(10):
        print(f"angle_index={i:3d} -> duty_cycle={spwm_get_duty_cycle(i, 80)}")

if __name__ == "__main__":
    main()