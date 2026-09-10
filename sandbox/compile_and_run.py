import subprocess
import os

# 尝试使用不同的编译器
compilers = ['gcc', 'clang', 'cc', 'mingw32-gcc']

for compiler in compilers:
    try:
        print(f"尝试使用 {compiler} 编译...")
        result = subprocess.run([compiler, '-Wall', '-std=c99', 'task_sliding_average.c', '-o', 'task_sliding_average.exe'], 
                              capture_output=True, text=True)
        if result.returncode == 0:
            print(f'使用 {compiler} 编译成功')
            # 运行程序
            print("运行程序...")
            result = subprocess.run(['./task_sliding_average.exe'], capture_output=True, text=True)
            print('程序输出:')
            print(result.stdout)
            if result.stderr:
                print('错误信息:')
                print(result.stderr)
            break
        else:
            print(f'使用 {compiler} 编译失败:')
            print(result.stderr)
    except FileNotFoundError:
        print(f'编译器 {compiler} 未找到')

# 如果没有找到编译器，尝试直接运行C代码
print("\n尝试直接运行C代码...")
try:
    result = subprocess.run(['python', '-c', '''
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int window_size;
    float *buffer;
    int index;
    int count;
    float sum;
} MovingAverageFilter;

void filter_init(MovingAverageFilter *filter, int window_size) {
    filter->window_size = window_size;
    filter->buffer = (float *)malloc(window_size * sizeof(float));
    filter->index = 0;
    filter->count = 0;
    filter->sum = 0.0f;
}

float filter_update(MovingAverageFilter *filter, float new_value) {
    if (filter->count == filter->window_size) {
        filter->sum -= filter->buffer[filter->index];
    } else {
        filter->count++;
    }
    
    filter->buffer[filter->index] = new_value;
    filter->sum += new_value;
    
    int next_index = filter->index + 1;
    if (next_index >= filter->window_size) {
        next_index = 0;
    }
    filter->index = next_index;
    
    return filter->sum / filter->count;
}

void filter_reset(MovingAverageFilter *filter) {
    filter->index = 0;
    filter->count = 0;
    filter->sum = 0.0f;
}

void filter_destroy(MovingAverageFilter *filter) {
    free(filter->buffer);
    filter->buffer = NULL;
}

int main() {
    MovingAverageFilter filter;
    
    filter_init(&filter, 5);
    
    float test_data[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0};
    int data_count = sizeof(test_data) / sizeof(test_data[0]);
    
    printf("测试滑动平均滤波器（窗口长度：5）\\n");
    printf("==================================\\n");
    
    for (int i = 0; i < data_count; i++) {
        float avg = filter_update(&filter, test_data[i]);
        printf("输入值: %.1f, 平均值: %.2f\\n", test_data[i], avg);
    }
    
    printf("\\n测试重置功能...\\n");
    filter_reset(&filter);
    
    float avg = filter_update(&filter, 8.0);
    printf("重置后输入值: 8.0, 平均值: %.2f\\n", avg);
    
    filter_destroy(&filter);
    
    printf("\\n测试完成，内存已释放。\\n");
    
    return 0;
}
'''], capture_output=True, text=True)
    print("直接运行结果:")
    print(result.stdout)
except:
    print("无法直接运行C代码")