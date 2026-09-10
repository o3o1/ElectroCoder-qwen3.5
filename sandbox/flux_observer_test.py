import sys

R_S = 0.5

psi_alpha_global = 0.0
psi_beta_global = 0.0

def flux_observer(u_alpha, u_beta, i_alpha, i_beta, dt):
    global psi_alpha_global, psi_beta_global
    u_alpha_comp = u_alpha - R_S * i_alpha
    u_beta_comp = u_beta - R_S * i_beta
    
    psi_alpha_global = psi_alpha_global + u_alpha_comp * dt
    psi_beta_global = psi_beta_global + u_beta_comp * dt
    
    return psi_alpha_global, psi_beta_global

dt = 0.001
test_inputs = [
    [100.0, 0.0, 10.0, 0.0],
    [0.0, 100.0, 0.0, 10.0],
    [-100.0, 0.0, -10.0, 0.0]
]

for step in range(3):
    psi_alpha, psi_beta = flux_observer(test_inputs[step][0], test_inputs[step][1], 
                                        test_inputs[step][2], test_inputs[step][3], dt)
    print(f'Step{step+1}: psi_alpha={psi_alpha:.3f}, psi_beta={psi_beta:.3f}')