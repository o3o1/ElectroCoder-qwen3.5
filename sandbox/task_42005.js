function iirLowpass(input, prevOutput, alpha) {
    const output = alpha * input + (1 - alpha) * prevOutput;
    return output;
}

function main() {
    const testSignal = [1.0, 1.2, 0.8, 1.5, 0.5, 1.3, 0.7, 1.1, 0.9, 1.0];
    const n = testSignal.length;
    
    process.stdout.write("原始信号: ");
    for (let i = 0; i < n; i++) {
        process.stdout.write(testSignal[i].toFixed(2) + " ");
    }
    process.stdout.write("\n");
    
    let prevOutput1 = testSignal[0];
    process.stdout.write("alpha=0.3滤波后: " + prevOutput1.toFixed(2) + " ");
    for (let i = 1; i < n; i++) {
        const filtered = iirLowpass(testSignal[i], prevOutput1, 0.3);
        prevOutput1 = filtered;
        process.stdout.write(filtered.toFixed(2) + " ");
    }
    process.stdout.write("\n");
    
    let prevOutput2 = testSignal[0];
    process.stdout.write("alpha=0.7滤波后: " + prevOutput2.toFixed(2) + " ");
    for (let i = 1; i < n; i++) {
        const filtered = iirLowpass(testSignal[i], prevOutput2, 0.7);
        prevOutput2 = filtered;
        process.stdout.write(filtered.toFixed(2) + " ");
    }
    process.stdout.write("\n");
}

main();