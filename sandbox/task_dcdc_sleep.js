// DC/DC变换器休眠唤醒控制模拟程序

// 模拟硬件寄存器宏定义
const SLEEP_THRESHOLD = 15;   // 休眠阈值(mA)
const WAKE_THRESHOLD = 25;    // 唤醒阈值(mA)

// 状态定义
const STATE_ACTIVE = 0;
const STATE_SLEEP = 1;

// 全局状态变量
let currentState = STATE_ACTIVE;
let transitionCount = 0;

// 模拟寄存器
let loadCurrentReg = 0;
let controlReg = 0;

function mockSetLoadCurrent(current) {
    // 模拟设置负载电流寄存器
    loadCurrentReg = current;
}

function mockGetLoadCurrent() {
    // 模拟读取负载电流寄存器
    return loadCurrentReg;
}

function mockSetControlReg(value) {
    // 模拟设置控制寄存器
    controlReg = value;
}

// DC/DC状态机函数
function dcdcStateMachine(loadCurrent) {
    // 设置负载电流到模拟寄存器
    mockSetLoadCurrent(loadCurrent);
    
    let newState = currentState;
    
    // 状态转换逻辑
    if (currentState === STATE_ACTIVE) {
        if (loadCurrent < SLEEP_THRESHOLD) {
            newState = STATE_SLEEP;
            mockSetControlReg(0x01);  // 设置休眠标志
        } else {
            mockSetControlReg(0x00);  // 保持活跃标志
        }
    } else { // STATE_SLEEP
        if (loadCurrent > WAKE_THRESHOLD) {
            newState = STATE_ACTIVE;
            mockSetControlReg(0x02);  // 设置唤醒标志
        } else {
            mockSetControlReg(0x01);  // 保持休眠标志
        }
    }
    
    // 如果状态发生变化，记录转换
    if (newState !== currentState) {
        if (currentState === STATE_ACTIVE && newState === STATE_SLEEP) {
            console.log(`状态: ACTIVE -> SLEEP, 电流: ${loadCurrent} mA`);
        } else if (currentState === STATE_SLEEP && newState === STATE_ACTIVE) {
            console.log(`状态: SLEEP -> ACTIVE, 电流: ${loadCurrent} mA`);
        }
        currentState = newState;
        transitionCount++;
    }
}

// 状态名称转换函数
function getStateName(state) {
    return state === STATE_ACTIVE ? "ACTIVE" : "SLEEP";
}

function main() {
    console.log("DC/DC变换器休眠唤醒控制模拟程序");
    console.log("================================\n");
    
    // 初始化状态为ACTIVE
    currentState = STATE_ACTIVE;
    transitionCount = 0;
    mockSetControlReg(0x00);  // 初始化为活跃状态
    
    // 测试用例1: 从高负载到低负载再到高负载
    console.log("测试用例1: 负载变化序列 [30, 10, 40] mA");
    const testCase1 = [30, 10, 40];
    testCase1.forEach(current => dcdcStateMachine(current));
    
    // 测试用例2: 在阈值附近波动
    console.log("\n测试用例2: 负载变化序列 [20, 14, 16, 26, 24] mA");
    const testCase2 = [20, 14, 16, 26, 24];
    testCase2.forEach(current => dcdcStateMachine(current));
    
    // 测试用例3: 长时间低负载后唤醒
    console.log("\n测试用例3: 负载变化序列 [5, 8, 12, 30, 35] mA");
    const testCase3 = [5, 8, 12, 30, 35];
    testCase3.forEach(current => dcdcStateMachine(current));
    
    // 打印最终结果
    console.log("\n================================\n");
    console.log(`最终状态: ${getStateName(currentState)}`);
    console.log(`总转换次数: ${transitionCount}`);
}

// 执行主函数
main();