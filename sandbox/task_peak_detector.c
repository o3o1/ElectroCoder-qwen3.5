#include <stdio.h>

typedef struct {
    float current_value;
    float peak_value;
    int hold_flag;
} PeakDetector;

void update_peak(PeakDetector* detector, float new_value) {
    detector->current_value = new_value;
    if (detector->hold_flag == 0 && new_value > detector->peak_value) {
        detector->peak_value = new_value;
    }
}

void reset_peak(PeakDetector* detector) {
    detector->peak_value = 0.0f;
    detector->hold_flag = 0;
}

void hold_peak(PeakDetector* detector) {
    detector->hold_flag = 1;
}

void release_peak(PeakDetector* detector) {
    detector->hold_flag = 0;
}

int main() {
    PeakDetector detector = {0.0f, 0.0f, 0};
    
    printf("=== 峰值检测器测试 ===\n\n");
    
    float input_sequence1[] = {1.2f, 3.4f, 2.8f, 4.5f, 3.9f};
    int len1 = sizeof(input_sequence1) / sizeof(input_sequence1[0]);
    
    printf("1. 初始输入序列: ");
    for (int i = 0; i < len1; i++) {
        printf("%.1f ", input_sequence1[i]);
    }
    printf("\n");
    
    for (int i = 0; i < len1; i++) {
        update_peak(&detector, input_sequence1[i]);
        printf("   输入: %.1f, 当前值: %.1f, 峰值: %.1f, 保持标志: %d\n", 
               input_sequence1[i], detector.current_value, detector.peak_value, detector.hold_flag);
    }
    
    printf("\n2. 在输入4.5后调用hold_peak()锁定峰值\n");
    hold_peak(&detector);
    printf("   当前值: %.1f, 峰值: %.1f, 保持标志: %d\n", 
           detector.current_value, detector.peak_value, detector.hold_flag);
    
    printf("\n3. 继续输入序列: 3.9, 5.0, 4.2 (峰值应保持为4.5)\n");
    float input_sequence2[] = {3.9f, 5.0f, 4.2f};
    int len2 = sizeof(input_sequence2) / sizeof(input_sequence2[0]);
    
    for (int i = 0; i < len2; i++) {
        update_peak(&detector, input_sequence2[i]);
        printf("   输入: %.1f, 当前值: %.1f, 峰值: %.1f, 保持标志: %d\n", 
               input_sequence2[i], detector.current_value, detector.peak_value, detector.hold_flag);
    }
    
    printf("\n4. 调用release_peak()释放保持\n");
    release_peak(&detector);
    printf("   当前值: %.1f, 峰值: %.1f, 保持标志: %d\n", 
           detector.current_value, detector.peak_value, detector.hold_flag);
    
    printf("\n5. 继续输入序列: 4.8, 5.5 (峰值应更新)\n");
    float input_sequence3[] = {4.8f, 5.5f};
    int len3 = sizeof(input_sequence3) / sizeof(input_sequence3[0]);
    
    for (int i = 0; i < len3; i++) {
        update_peak(&detector, input_sequence3[i]);
        printf("   输入: %.1f, 当前值: %.1f, 峰值: %.1f, 保持标志: %d\n", 
               input_sequence3[i], detector.current_value, detector.peak_value, detector.hold_flag);
    }
    
    printf("\n6. 调用reset_peak()清零\n");
    reset_peak(&detector);
    printf("   当前值: %.1f, 峰值: %.1f, 保持标志: %d\n", 
           detector.current_value, detector.peak_value, detector.hold_flag);
    
    printf("\n=== 测试完成 ===\n");
    
    return 0;
}