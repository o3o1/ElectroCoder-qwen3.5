Option Explicit

Const EPSILON = 0.01
Const BATTERY_VOLTAGE_NOMINAL = 12.0
Const BATTERY_VOLTAGE_MIN = 10.5
Const BATTERY_VOLTAGE_MAX = 14.5
Const BATTERY_VOLTAGE_FLOAT = 13.8
Const BATTERY_VOLTAGE_CUTOFF = 11.0
Const BATTERY_VOLTAGE_RESTORE = 11.5
Const BATTERY_CAPACITY = 7.0
Const PWM_MAX = 1.0
Const PWM_MIN = 0.0
Const PWM_STEP = 0.05

Class SolarControllerState
    Public V_pv
    Public I_pv
    Public P_pv
    Public D
    Public V_bat
    Public charging_state
    Public load_state
    
    Private Sub Class_Initialize()
        V_pv = 0.0
        I_pv = 0.0
        P_pv = 0.0
        D = 0.0
        V_bat = 0.0
        charging_state = 0
        load_state = 0
    End Sub
End Class

Function calculate_pv_current(V_pv, k)
    If V_pv < 0 Or V_pv > 24.0 Then
        calculate_pv_current = 0.0
    Else
        calculate_pv_current = k * V_pv * (1.0 - V_pv / 24.0)
    End If
End Function

Sub mppt_incremental_conductance(state, prev_state, k)
    Dim delta_P, delta_V, dP_dV, P_V_ratio
    
    delta_P = state.P_pv - prev_state.P_pv
    delta_V = state.V_pv - prev_state.V_pv
    
    If Abs(delta_V) < EPSILON Then
        If delta_P > 0 Then
            state.D = state.D + PWM_STEP
        Else
            state.D = state.D - PWM_STEP
        End If
    Else
        dP_dV = delta_P / delta_V
        P_V_ratio = -state.P_pv / state.V_pv
        
        If dP_dV > P_V_ratio Then
            state.D = state.D + PWM_STEP
        Else
            state.D = state.D - PWM_STEP
        End If
    End If
    
    If state.D > PWM_MAX Then state.D = PWM_MAX
    If state.D < PWM_MIN Then state.D = PWM_MIN
    
    state.V_pv = 24.0 * state.D
    state.I_pv = calculate_pv_current(state.V_pv, k)
    state.P_pv = state.V_pv * state.I_pv
End Sub

Sub update_battery_state(state, delta_time)
    Dim charge_current, load_current, net_current, delta_V
    
    charge_current = 0.0
    load_current = 0.0
    
    If state.P_pv > 0 And state.V_bat < BATTERY_VOLTAGE_MAX Then
        If state.V_bat < BATTERY_VOLTAGE_FLOAT Then
            charge_current = 1.0
            state.charging_state = 1
        Else
            charge_current = 0.1
            state.charging_state = 2
        End If
    Else
        state.charging_state = 0
    End If
    
    If state.V_bat < BATTERY_VOLTAGE_CUTOFF Then
        state.load_state = 0
        load_current = 0.0
    ElseIf state.V_bat >= BATTERY_VOLTAGE_RESTORE Then
        state.load_state = 1
        load_current = 0.5
    End If
    
    net_current = charge_current - load_current
    delta_V = net_current * delta_time / BATTERY_CAPACITY
    state.V_bat = state.V_bat + delta_V
    
    If state.V_bat < BATTERY_VOLTAGE_MIN Then state.V_bat = BATTERY_VOLTAGE_MIN
    If state.V_bat > BATTERY_VOLTAGE_MAX Then state.V_bat = BATTERY_VOLTAGE_MAX
End Sub

Sub print_state(iteration, state, k)
    Dim charging_text(2), load_text
    
    charging_text(0) = "充电:关"
    charging_text(1) = "充电:恒流"
    charging_text(2) = "充电:浮充"
    
    If state.load_state = 1 Then
        load_text = "开"
    Else
        load_text = "关"
    End If
    
    WScript.Echo "光照k=" & FormatNumber(k, 1) & ", 迭代" & iteration & ": V_pv=" & FormatNumber(state.V_pv, 2) & "V, I_pv=" & FormatNumber(state.I_pv, 2) & "A, P_pv=" & FormatNumber(state.P_pv, 2) & "W, D=" & FormatNumber(state.D, 2) & ", V_bat=" & FormatNumber(state.V_bat, 2) & "V, " & charging_text(state.charging_state) & ", 负载:" & load_text
End Sub

Sub main()
    Dim k_values(3), iterations_per_k
    Dim current_state, prev_state
    Dim k_idx, iter, k
    
    k_values(0) = 0.3
    k_values(1) = 0.7
    k_values(2) = 0.9
    k_values(3) = 0.2
    iterations_per_k = 5
    
    Set current_state = New SolarControllerState
    Set prev_state = New SolarControllerState
    
    current_state.V_bat = 12.0
    current_state.D = 0.5
    current_state.charging_state = 0
    current_state.load_state = 1
    
    WScript.Echo "=== 太阳能路灯控制器模拟 ==="
    
    For k_idx = 0 To 3
        k = k_values(k_idx)
        WScript.Echo vbCrLf & "--- 光照强度 k=" & FormatNumber(k, 1) & " ---"
        
        current_state.V_pv = 24.0 * current_state.D
        current_state.I_pv = calculate_pv_current(current_state.V_pv, k)
        current_state.P_pv = current_state.V_pv * current_state.I_pv
        
        For iter = 0 To iterations_per_k - 1
            prev_state.V_pv = current_state.V_pv
            prev_state.I_pv = current_state.I_pv
            prev_state.P_pv = current_state.P_pv
            prev_state.D = current_state.D
            prev_state.V_bat = current_state.V_bat
            prev_state.charging_state = current_state.charging_state
            prev_state.load_state = current_state.load_state
            
            If iter > 0 Then
                mppt_incremental_conductance current_state, prev_state, k
            End If
            
            update_battery_state current_state, 1.0
            print_state iter + 1, current_state, k
        Next
    Next
    
    WScript.Echo vbCrLf & "=== 电池状态总结 ==="
    WScript.Echo "最终电池电压: " & FormatNumber(current_state.V_bat, 2) & "V"
    
    Dim charging_text(2)
    charging_text(0) = "未充电"
    charging_text(1) = "恒流充电"
    charging_text(2) = "浮充"
    
    WScript.Echo "充电状态: " & charging_text(current_state.charging_state)
    
    If current_state.load_state = 1 Then
        WScript.Echo "负载状态: 开启"
    Else
        WScript.Echo "负载状态: 关闭"
    End If
End Sub

main