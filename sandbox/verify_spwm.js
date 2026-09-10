// JavaScript验证SPWM算法逻辑
const PWM_MAX_VALUE = 1000;
const SINE_TABLE_SIZE = 256;

let sine_table = new Array(SINE_TABLE_SIZE);

function spwm_init() {
    for (let i = 0; i < SINE_TABLE_SIZE; i++) {
        const angle = 2.0 * Math.PI * i / SINE_TABLE_SIZE;
        sine_table[i] = (Math.sin(angle) + 1.0) / 2.0;
    }
}

function spwm_get_duty_cycle(angle_index, modulation_index) {
    const sine_value = sine_table[angle_index];
    const modulation = modulation_index / 100.0;
    const duty_ratio = sine_value * modulation;
    return Math.round(duty_ratio * PWM_MAX_VALUE);
}

// 执行验证
spwm_init();

console.log("SPWM test results:");
console.log(`1. angle_index=0, modulation_index=50 -> duty_cycle=${spwm_get_duty_cycle(0, 50)}`);
console.log(`2. angle_index=64, modulation_index=75 -> duty_cycle=${spwm_get_duty_cycle(64, 75)}`);
console.log(`3. angle_index=128, modulation_index=100 -> duty_cycle=${spwm_get_duty_cycle(128, 100)}`);

console.log("\nFirst 10 points at 80% modulation:");
for (let i = 0; i < 10; i++) {
    console.log(`angle_index=${i.toString().padStart(3)} -> duty_cycle=${spwm_get_duty_cycle(i, 80)}`);
}

// 验证正弦表正确性
console.log("\nSine table verification:");
console.log(`sine_table[0] = ${sine_table[0].toFixed(6)} (should be ~0.5)`);
console.log(`sine_table[64] = ${sine_table[64].toFixed(6)} (quarter cycle)`);
console.log(`sine_table[128] = ${sine_table[128].toFixed(6)} (half cycle, should be ~1.0)`);
console.log(`sine_table[192] = ${sine_table[192].toFixed(6)} (three-quarter cycle)`);