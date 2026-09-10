# OBC Control Algorithm Test  
  
import sys  
  
def obc_control(pfc_output, llc_output, grid_voltage, battery_voltage, battery_soc):  
    PFC_MAX_VOLTAGE = 450.0  
    LLC_MAX_POWER = 5000.0  
    GRID_VOLTAGE_MIN = 176.0  
    GRID_VOLTAGE_MAX = 264.0  
    PFC_TARGET_VOLTAGE = 400.0  
    LLC_CONSTANT_CURRENT = 10.0  
    LLC_CHARGE_MARGIN = 0.5  
  
    pfc_target = PFC_TARGET_VOLTAGE  
  
