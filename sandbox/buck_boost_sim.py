import math

VIN = 24.0
VREF = -12.0
I_MAX = 5.0
FSW = 100000.0
T = 1.0/FSW
L = 100e-6
C = 470e-6
R_LOAD = 6.0

KP_I = 0.05
KP_V = 0.1
KI_V = 15.0
INTEGRAL_LIMIT = 2.0
DUTY_MIN = 0.0
DUTY_MAX = 0.9

def voltage_pi_controller(error, integral):
    integral_term = integral + KI_V * error * T
    
    if integral_term > INTEGRAL_LIMIT: 
        integral_term = INTEGRAL_LIMIT
    if integral_term < -INTEGRAL_LIMIT: 
        integral_term = -INTEGRAL_LIMIT
    
    output = KP_V * error + integral_term
    
    if output > I_MAX: 
        output = I_MAX
    if output < -I_MAX: 
        output = -I_MAX
    
    return output, integral_term

def current_p_controller(error):
    return KP_I * error

def calculate_duty(inductor_current, voltage_pi_output):
    current_error = voltage_pi_output - inductor_current
    duty_increment = current_p_controller(current_error)
    
    duty = 0.5 + duty_increment
    
    if duty > DUTY_MAX: 
        duty = DUTY_MAX
    if duty < DUTY_MIN: 
        duty = DUTY_MIN
    
    return duty

def update_circuit_state(duty, Vout, I_L):
    I_L_new = I_L + (VIN * duty / L - Vout * (1 - duty) / L) * T
    Vout_new = Vout + (I_L * (1 - duty) - Vout / R_LOAD) * T / C
    
    if I_L_new > I_MAX: 
        I_L_new = I_MAX
    if I_L_new < -I_MAX: 
        I_L_new = -I_MAX
    
    return Vout_new, I_L_new

def main():
    Vout = 0.0
    I_L = 0.0
    integral = 0.0
    
    print("Buck-Boost变换器双环控制仿真")
    print("周期\t输出电压(V)\t电感电流(A)\t占空比")
    print("------------------------------------------------")
    
    for cycle in range(1000):
        error = VREF - Vout
        
        pi_output, integral = voltage_pi_controller(error, integral)
        
        duty = calculate_duty(I_L, pi_output)
        
        Vout, I_L = update_circuit_state(duty, Vout, I_L)
        
        if (cycle % 100 == 0) or cycle == 999:
            print(f"{cycle}\t{Vout:.3f}\t\t{I_L:.3f}\t\t{duty:.3f}")
    
    print("------------------------------------------------")
    print("仿真完成")

if __name__ == "__main__":
    main()