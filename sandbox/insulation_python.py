#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import math

def calculate_insulation_resistance(Vdc, Vp, Vn, R1, R2):
    """Calculate insulation resistance using balanced bridge method"""
    # Calculate positive insulation resistance
    if Vp > 0.001:  # Avoid division by zero
        Rp = R1 * (Vdc - Vp) / Vp - R2
        if Rp < 0:
            Rp = 0  # Insulation resistance cannot be negative
    else:
        Rp = float('inf')  # Perfect insulation
    
    # Calculate negative insulation resistance
    if Vn > 0.001:  # Avoid division by zero
        Rn = R1 * (Vdc - Vn) / Vn - R2
        if Rn < 0:
            Rn = 0  # Insulation resistance cannot be negative
    else:
        Rn = float('inf')  # Perfect insulation
    
    return Rp, Rn

def main():
    # Test cases data
    test_cases = [
        # (Vdc, Vp, Vn, R1, R2)
        (400.0, 200.0, 200.0, 1000.0, 1000.0),  # Test case 1
        (500.0, 300.0, 250.0, 1500.0, 1200.0),  # Test case 2
        (600.0, 400.0, 350.0, 2000.0, 1800.0)   # Test case 3
    ]
    
    print("DC Charging Pile Insulation Detection - Balanced Bridge Algorithm")
    print("=================================================================")
    print()
    
    # Run three test cases
    for i, (Vdc, Vp, Vn, R1, R2) in enumerate(test_cases, 1):
        # Calculate insulation resistance
        Rp, Rn = calculate_insulation_resistance(Vdc, Vp, Vn, R1, R2)
        
        # Print results
        print(f"Test Case {i}: Vdc={Vdc:.2f}V, Vp={Vp:.2f}V, Vn={Vn:.2f}V => Rp={Rp:.2f}Ω, Rn={Rn:.2f}Ω")
    
    # Additional test case with perfect insulation (Vp = Vn = Vdc/2)
    print("\nAdditional Test: Ideal Insulation Case")
    Vdc_test = 400.0
    Vp_test = 200.0
    Vn_test = 200.0
    R1_test = 1000.0
    R2_test = 1000.0
    
    Rp_test, Rn_test = calculate_insulation_resistance(Vdc_test, Vp_test, Vn_test, R1_test, R2_test)
    print(f"Vdc={Vdc_test:.2f}V, Vp={Vp_test:.2f}V, Vn={Vn_test:.2f}V => Rp={Rp_test:.2f}Ω, Rn={Rn_test:.2f}Ω")

if __name__ == "__main__":
    main()