def scale_voltage(pt_voltage, pt_ratio, input_range_max, output_range_max):
    if input_range_max == 0.0:
        return 0.0
    return (pt_voltage / input_range_max) * output_range_max

def main():
    test_cases = [
        (2.5, 100.0, 5.0, 3.3),
        (0.0, 100.0, 5.0, 3.3),
        (5.0, 100.0, 5.0, 3.3),
        (3.0, 50.0, 10.0, 5.0)
    ]
    
    for pt_voltage, pt_ratio, input_range_max, output_range_max in test_cases:
        scaled_voltage = scale_voltage(pt_voltage, pt_ratio, input_range_max, output_range_max)
        print(f"PT电压: {pt_voltage:.2f}V, 变比: {pt_ratio:.0f}, 输入量程: {input_range_max:.2f}V, 输出量程: {output_range_max:.2f}V -> 适配后: {scaled_voltage:.2f}V")

if __name__ == "__main__":
    main()