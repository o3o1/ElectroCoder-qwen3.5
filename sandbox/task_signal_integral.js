// 电力电子信号积分计算 - 梯形积分法
console.log("电力电子信号积分计算 - 梯形积分法");
console.log("===================================\n");

function trapezoidal_integral(x, y) {
    if (x.length < 2 || y.length < 2 || x.length !== y.length) {
        return 0.0;
    }
    
    let integral = 0.0;
    for (let i = 1; i < x.length; i++) {
        const dx = x[i] - x[i-1];
        const avg_y = (y[i] + y[i-1]) / 2.0;
        integral += avg_y * dx;
    }
    return integral;
}

function test_constant_function() {
    console.log("=== 测试1: 常数函数 y=2.0 在区间[0,5] ===");
    
    const x = [0.0, 1.0, 2.0, 3.0, 4.0, 5.0];
    const y = [2.0, 2.0, 2.0, 2.0, 2.0, 2.0];
    
    console.log(`时间数组 x: ${x.map(v => v.toFixed(1)).join(' ')}`);
    console.log(`信号数组 y: ${y.map(v => v.toFixed(1)).join(' ')}`);
    
    const result = trapezoidal_integral(x, y);
    const expected = 10.0;
    const error = Math.abs(result - expected);
    
    console.log(`计算结果: ${result.toFixed(6)}`);
    console.log(`理论值: ${expected.toFixed(6)}`);
    console.log(`误差: ${error.toFixed(6)}\n`);
}

function test_linear_function() {
    console.log("=== 测试2: 线性函数 y=x 在区间[0,3] ===");
    
    const x = [0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0];
    const y = [0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0];
    
    console.log(`时间数组 x: ${x.map(v => v.toFixed(1)).join(' ')}`);
    console.log(`信号数组 y: ${y.map(v => v.toFixed(1)).join(' ')}`);
    
    const result = trapezoidal_integral(x, y);
    const expected = 4.5;
    const error = Math.abs(result - expected);
    
    console.log(`计算结果: ${result.toFixed(6)}`);
    console.log(`理论值: ${expected.toFixed(6)}`);
    console.log(`误差: ${error.toFixed(6)}\n`);
}

function test_sine_function() {
    console.log("=== 测试3: 正弦函数 y=sin(x) 在区间[0,π] ===");
    
    const n = 11;
    const x = [];
    const y = [];
    
    for (let i = 0; i < n; i++) {
        x[i] = i * Math.PI / (n - 1);
        y[i] = Math.sin(x[i]);
    }
    
    console.log(`时间数组 x: ${x.map(v => v.toFixed(3)).join(' ')}`);
    console.log(`信号数组 y: ${y.map(v => v.toFixed(3)).join(' ')}`);
    
    const result = trapezoidal_integral(x, y);
    const expected = 2.0;
    const error = Math.abs(result - expected);
    
    console.log(`计算结果: ${result.toFixed(6)}`);
    console.log(`理论值: ${expected.toFixed(6)}`);
    console.log(`误差: ${error.toFixed(6)}\n`);
}

// 运行测试
test_constant_function();
test_linear_function();
test_sine_function();