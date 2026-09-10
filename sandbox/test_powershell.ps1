Add-Type -TypeDefinition @"
using System;
using System.Runtime.InteropServices;

public class DllImporter {
    [DllImport("msvcrt.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern int printf(string format);
    
    [DllImport("msvcrt.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern double sin(double x);
}
"@

[DllImporter]::printf("Test\n")