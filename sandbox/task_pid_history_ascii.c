#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_RECORDS 100

typedef struct {
    unsigned long timestamp;
    float Kp;
    float Ki;
    float Kd;
    float error;
    float output;
} PID_Record;

void add_record(PID_Record records[], int *count, float Kp, float Ki, float Kd, float error, float output) {
    if (*count >= MAX_RECORDS) {
        return;
    }
    
    static unsigned long last_timestamp = 0;
    records[*count].timestamp = last_timestamp + 100;
    last_timestamp = records[*count].timestamp;
    
    records[*count].Kp = Kp;
    records[*count].Ki = Ki;
    records[*count].Kd = Kd;
    records[*count].error = error;
    records[*count].output = output;
    
    (*count)++;
}

void print_trend(PID_Record records[], int count) {
    printf("=== PID参数历史记录 ===\n");
    printf("时间戳(ms) | Kp     | Ki     | Kd     | 误差    | 输出\n");
    printf("--------------------------------------------------------\n");
    
    for (int i = 0; i < count; i++) {
        printf("%10lu | %6.2f | %6.2f | %6.2f | %7.2f | %6.2f\n",
               records[i].timestamp,
               records[i].Kp,
               records[i].Ki,
               records[i].Kd,
               records[i].error,
               records[i].output);
    }
    
    if (count >= 3) {
        float moving_avg = (records[count-1].output + records[count-2].output + records[count-3].output) / 3.0f;
        printf("\n移动平均输出：%.2f\n", moving_avg);
    } else if (count > 0) {
        float sum = 0;
        for (int i = 0; i < count; i++) {
            sum += records[i].output;
        }
        printf("\n移动平均输出：%.2f\n", sum / count);
    }
}

void analyze_overshoot(PID_Record records[], int count) {
    printf("\n=== 超调分析 ===\n");
    
    int overshoot_count = 0;
    const float setpoint = 0.0f;
    const float overshoot_threshold = 0.1f;
    
    for (int i = 0; i < count; i++) {
        float target_error = setpoint - records[i].error;
        float overshoot_percentage = (records[i].output - target_error) / target_error;
        
        if (target_error != 0 && overshoot_percentage > overshoot_threshold) {
            if (overshoot_count == 0) {
                printf("发现超调记录：\n");
                printf("时间戳(ms) | 输出   | 目标误差 | 超调百分比\n");
                printf("--------------------------------------------\n");
            }
            printf("%10lu | %6.2f | %8.2f | %10.1f%%\n",
                   records[i].timestamp,
                   records[i].output,
                   target_error,
                   overshoot_percentage * 100);
            overshoot_count++;
        }
    }
    
    if (overshoot_count == 0) {
        printf("无超调记录\n");
    }
}

int main() {
    PID_Record records[MAX_RECORDS];
    int count = 0;
    
    add_record(records, &count, 1.0f, 0.1f, 0.01f, -5.0f, 25.0f);
    add_record(records, &count, 1.2f, 0.15f, 0.02f, -3.0f, 18.0f);
    add_record(records, &count, 1.5f, 0.2f, 0.03f, -1.5f, 12.0f);
    add_record(records, &count, 1.8f, 0.25f, 0.04f, -0.5f, 8.0f);
    add_record(records, &count, 2.0f, 0.3f, 0.05f, 0.2f, 5.0f);
    
    print_trend(records, count);
    analyze_overshoot(records, count);
    
    return 0;
}