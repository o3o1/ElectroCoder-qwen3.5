const EPSILON = 0.01;
const BATTERY_VOLTAGE_NOMINAL = 12.0;
const BATTERY_VOLTAGE_MIN = 10.5;
const BATTERY_VOLTAGE_MAX = 14.5;
const BATTERY_VOLTAGE_FLOAT = 13.8;
const BATTERY_VOLTAGE_CUTOFF = 11.0;
const BATTERY_VOLTAGE_RESTORE = 11.5;
const BATTERY_CAPACITY = 7.0;
const PWM_MAX = 1.0;
const PWM_MIN = 0.0;
const PWM_STEP = 0.05;

class SolarControllerState {
    constructor() {
        this.V_pv = 0.0;
        this.I_pv = 0.0;
        this.P_pv = 0.0;
        this.D = 0.0;
        this.V_bat = 0.0;
        this.charging_state = 0;
        this.load_state = 0;
    }
}

function calculate_pv_current(V_pv, k) {
    if (V_pv < 0 || V_pv > 24.0) return 0.0;
    return k * V_pv * (1.0 - V_pv / 24.0);
}

function mppt_incremental_conductance(state, prev_state, k) {
    const delta_P = state.P_pv - prev_state.P_pv;
    const delta_V = state.V_pv - prev_state.V_pv;
    
    if (Math.abs(delta_V) < EPSILON) {
        if (delta_P > 0) {
            state.D += PWM_STEP;
        } else {
            state.D -= PWM_STEP;
        }
    } else {
        const dP_dV = delta_P / delta_V;
        const P_V_ratio = -state.P_pv / state.V_pv;
        
        if (dP_dV > P_V_ratio) {
            state.D += PWM_STEP;
        } else {
            state.D -= PWM_STEP;
        }
    }
    
    if (state.D > PWM_MAX) state.D = PWM_MAX;
    if (state.D < PWM_MIN) state.D = PWM_MIN;
    
    state.V_pv = 24.0 * state.D;
    state.I_pv = calculate_pv_current(state.V_pv, k);
    state.P_pv = state.V_pv * state.I_pv;
}

function update_battery_state(state, delta_time) {
    let charge_current = 0.0;
    let load_current = 0.0;
    
    if (state.P_pv > 0 && state.V_bat < BATTERY_VOLTAGE_MAX) {
        if (state.V_bat < BATTERY_VOLTAGE_FLOAT) {
            charge_current = 1.0;
            state.charging_state = 1;
        } else {
            charge_current = 0.1;
            state.charging_state = 2;
        }
    } else {
        state.charging_state = 0;
    }
    
    if (state.V_bat < BATTERY_VOLTAGE_CUTOFF) {
        state.load_state = 0;
        load_current = 0.0;
    } else if (state.V_bat >= BATTERY_VOLTAGE_RESTORE) {
        state.load_state = 1;
        load_current = 0.5;
    }
    
    const net_current = charge_current - load_current;
    const delta_V = net_current * delta_time / BATTERY_CAPACITY;
    state.V_bat += delta_V;
    
    if (state.V_bat < BATTERY_VOLTAGE_MIN) state.V_bat = BATTERY_VOLTAGE_MIN;
    if (state.V_bat > BATTERY_VOLTAGE_MAX) state.V_bat = BATTERY_VOLTAGE_MAX;
}

function print_state(iteration, state, k) {
    const charging_text = {0: '充电:关', 1: '充电:恒流', 2: '充电:浮充'};
    const load_text = state.load_state ? '开' : '关';
    
    console.log(`光照k=${k.toFixed(1)}, 迭代${iteration}: V_pv=${state.V_pv.toFixed(2)}V, I_pv=${state.I_pv.toFixed(2)}A, P_pv=${state.P_pv.toFixed(2)}W, D=${state.D.toFixed(2)}, V_bat=${state.V_bat.toFixed(2)}V, ${charging_text[state.charging_state]}, 负载:${load_text}`);
}

function main() {
    const k_values = [0.3, 0.7, 0.9, 0.2];
    const iterations_per_k = 5;
    
    const current_state = new SolarControllerState();
    const prev_state = new SolarControllerState();
    
    current_state.V_bat = 12.0;
    current_state.D = 0.5;
    current_state.charging_state = 0;
    current_state.load_state = 1;
    
    console.log('=== 太阳能路灯控制器模拟 ===');
    
    for (const k of k_values) {
        console.log(`\n--- 光照强度 k=${k.toFixed(1)} ---`);
        
        current_state.V_pv = 24.0 * current_state.D;
        current_state.I_pv = calculate_pv_current(current_state.V_pv, k);
        current_state.P_pv = current_state.V_pv * current_state.I_pv;
        
        for (let iter = 0; iter < iterations_per_k; iter++) {
            Object.assign(prev_state, JSON.parse(JSON.stringify(current_state)));
            
            if (iter > 0) {
                mppt_incremental_conductance(current_state, prev_state, k);
            }
            
            update_battery_state(current_state, 1.0);
            print_state(iter + 1, current_state, k);
        }
    }
    
    console.log('\n=== 电池状态总结 ===');
    console.log(`最终电池电压: ${current_state.V_bat.toFixed(2)}V`);
    
    const charging_text = {0: '未充电', 1: '恒流充电', 2: '浮充'};
    console.log(`充电状态: ${charging_text[current_state.charging_state]}`);
    console.log(`负载状态: ${current_state.load_state ? '开启' : '关闭'}`);
}

main();