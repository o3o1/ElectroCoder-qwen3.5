#include <stdio.h>

typedef struct {
    int id;
    float voltage;
    float current;
    float power;
} ChannelData;

void read_channel_data(ChannelData *ch, int id) {
    ch->id = id;
    ch->voltage = 220.0f + id * 0.5f;
    ch->current = 10.0f + id * 0.2f;
    ch->power = ch->voltage * ch->current;
}

void read_all_channels(ChannelData channels[], int num_channels) {
    for (int i = 0; i < num_channels; i++) {
        read_channel_data(&channels[i], i);
    }
}

float calculate_total_power(ChannelData channels[], int num_channels) {
    float total = 0.0f;
    for (int i = 0; i < num_channels; i++) {
        total += channels[i].power;
    }
    return total;
}

void print_channel_report(ChannelData channels[], int num_channels) {
    for (int i = 0; i < num_channels; i++) {
        printf("通道%d: 电压=%.1fV, 电流=%.1fA, 功率=%.1fW\n", 
               channels[i].id, channels[i].voltage, channels[i].current, channels[i].power);
    }
}

int main() {
    const int NUM_CHANNELS = 8;
    ChannelData channels[NUM_CHANNELS];
    
    read_all_channels(channels, NUM_CHANNELS);
    print_channel_report(channels, NUM_CHANNELS);
    
    float total_power = calculate_total_power(channels, NUM_CHANNELS);
    printf("总功率: %.1fW\n", total_power);
    
    return 0;
}