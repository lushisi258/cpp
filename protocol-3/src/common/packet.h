#ifndef PACKET_H
#define PACKET_H

#include <cstring>

// 数据包结构
struct Packet {
    int seq_num;      // 序列号
    int checksum;     // 校验和
    char data[12288];  // 数据内容
};

// 计算校验和
int calculate_checksum(Packet &packet);

#endif // PACKET_H
