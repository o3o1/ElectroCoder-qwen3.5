@echo off
echo Running flux observer calculation...
echo Step 1: u_alpha=100.0, u_beta=0.0, i_alpha=10.0, i_beta=0.0
echo u_alpha_comp = 100.0 - 0.5 * 10.0 = 95.0
echo psi_alpha = 0.0 + 95.0 * 0.001 = 0.095
echo psi_beta = 0.0 + 0.0 * 0.001 = 0.0
echo.
echo Step 2: u_alpha=0.0, u_beta=100.0, i_alpha=0.0, i_beta=10.0
echo u_beta_comp = 100.0 - 0.5 * 10.0 = 95.0
echo psi_alpha = 0.095 + 0.0 * 0.001 = 0.095
echo psi_beta = 0.0 + 95.0 * 0.001 = 0.095
echo.
echo Step 3: u_alpha=-100.0, u_beta=0.0, i_alpha=-10.0, i_beta=0.0
echo u_alpha_comp = -100.0 - 0.5 * -10.0 = -95.0
echo psi_alpha = 0.095 + -95.0 * 0.001 = 0.0
echo psi_beta = 0.095 + 0.0 * 0.001 = 0.095
echo.
echo Final results:
echo psi_alpha = 0.0
echo psi_beta = 0.095