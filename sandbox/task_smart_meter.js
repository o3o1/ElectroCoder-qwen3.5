const SAMPLE_COUNT = 10;
const SAMPLE_FREQ = 1000;
const SAMPLE_INTERVAL = 0.001;
const VOLTAGE_PEAK = 311.0;
const CURRENT = 5.0;
const PULSE_THRESHOLD = 0.1;
const PI = 3.14159265358979323846;

function calculateRMS(samples) {
    let sumSq = 0.0;
    for (let sample of samples) {
        sumSq += sample * sample;
    }
    return Math.sqrt(sumSq / samples.length);
}

function generateBalancedSamples() {
    const phaseA = [];
    const phaseB = [];
    const phaseC = [];
    
    for (let i = 0; i < SAMPLE_COUNT; i++) {
        const angle = 2 * PI * i / SAMPLE_COUNT;
        phaseA.push(VOLTAGE_PEAK * Math.sin(angle));
        phaseB.push(VOLTAGE_PEAK * Math.sin(angle - 2 * PI / 3));
        phaseC.push(VOLTAGE_PEAK * Math.sin(angle + 2 * PI / 3));
    }
    
    return { phaseA, phaseB, phaseC };
}

function generateUnbalancedSamples() {
    const phaseA = [];
    const phaseB = [];
    const phaseC = [];
    
    for (let i = 0; i < SAMPLE_COUNT; i++) {
        const angle = 2 * PI * i / SAMPLE_COUNT;
        phaseA.push(1.136 * VOLTAGE_PEAK * Math.sin(angle));
        phaseB.push(VOLTAGE_PEAK * Math.sin(angle - 2 * PI / 3));
        phaseC.push(VOLTAGE_PEAK * Math.sin(angle + 2 * PI / 3));
    }
    
    return { phaseA, phaseB, phaseC };
}

function simulateEnergyAccumulation(uaRMS, ubRMS, ucRMS, testCase) {
    let totalEnergy = 0.0;
    let energySinceLastPulse = 0.0;
    let pulseCount = 0;
    
    console.log(`测试用例${testCase} - 能量累计模拟:`);
    
    while (totalEnergy < 0.5) {
        const power = (uaRMS + ubRMS + ucRMS) * CURRENT;
        const energyIncrement = power * SAMPLE_INTERVAL / 3600000.0;
        
        totalEnergy += energyIncrement;
        energySinceLastPulse += energyIncrement;
        
        if (energySinceLastPulse >= PULSE_THRESHOLD) {
            pulseCount++;
            console.log(`  脉冲事件${pulseCount}: 累计电量 ${totalEnergy.toFixed(3)} kWh`);
            energySinceLastPulse = 0.0;
        }
    }
    
    console.log(`  总累计电量: ${totalEnergy.toFixed(3)} kWh, 总脉冲数: ${pulseCount}\n`);
}

function main() {
    console.log("智能电表模拟程序");
    console.log("================");
    console.log();
    
    console.log("测试用例1: 三相平衡情况");
    const balanced = generateBalancedSamples();
    
    const uaRMS1 = calculateRMS(balanced.phaseA);
    const ubRMS1 = calculateRMS(balanced.phaseB);
    const ucRMS1 = calculateRMS(balanced.phaseC);
    
    console.log(`  A相电压有效值: ${uaRMS1.toFixed(2)} V`);
    console.log(`  B相电压有效值: ${ubRMS1.toFixed(2)} V`);
    console.log(`  C相电压有效值: ${ucRMS1.toFixed(2)} V`);
    
    simulateEnergyAccumulation(uaRMS1, ubRMS1, ucRMS1, 1);
    
    console.log("测试用例2: A相电压偏高情况");
    const unbalanced = generateUnbalancedSamples();
    
    const uaRMS2 = calculateRMS(unbalanced.phaseA);
    const ubRMS2 = calculateRMS(unbalanced.phaseB);
    const ucRMS2 = calculateRMS(unbalanced.phaseC);
    
    console.log(`  A相电压有效值: ${uaRMS2.toFixed(2)} V`);
    console.log(`  B相电压有效值: ${ubRMS2.toFixed(2)} V`);
    console.log(`  C相电压有效值: ${ucRMS2.toFixed(2)} V`);
    
    simulateEnergyAccumulation(uaRMS2, ubRMS2, ucRMS2, 2);
}

main();