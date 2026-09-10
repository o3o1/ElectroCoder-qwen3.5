@echo off
echo Running flux observer test...
echo R_S = 0.5
echo psi_alpha_global = 0.0
echo psi_beta_global = 0.0
echo.
echo Test 1: u_alpha=100.0, u_beta=0.0, i_alpha=10.0, i_beta=0.0
set /a u_alpha_comp = 100.0 - 0.5 * 10.0
set /a psi_alpha = 0.0 + %u_alpha_comp% * 0.001
set /a psi_beta = 0.0 + 0.0 * 0.001
echo Step1: psi_alpha=%psi_alpha%, psi_beta=%psi_beta%
echo.
echo Test 2: u_alpha=0.0, u_beta=100.0, i_alpha=0.0, i_beta=10.0
set /a u_beta_comp = 100.0 - 0.5 * 10.0
set /a psi_alpha = %psi_alpha% + 0.0 * 0.001
set /a psi_beta = %psi_beta% + %u_beta_comp% * 0.001
echo Step2: psi_alpha=%psi_alpha%, psi_beta=%psi_beta%
echo.
echo Test 3: u_alpha=-100.0, u_beta=0.0, i_alpha=-10.0, i_beta=0.0
set /a u_alpha_comp = -100.0 - 0.5 * -10.0
set /a psi_alpha = %psi_alpha% + %u_alpha_comp% * 0.001
set /a psi_beta = %psi_beta% + 0.0 * 0.001
echo Step3: psi_alpha=%psi_alpha%, psi_beta=%psi_beta%