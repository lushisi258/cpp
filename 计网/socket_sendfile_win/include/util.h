#ifndef UTIL_H
#define UTIL_H

#include <cstdint>
#include <cstring>

#define MAX_PAYLOAD 4096
#define WINDOW_SIZE 30
#define TIMEOUT_MS 200

enum PacketType { PKT_SYN = 1, PKT_SYN_ACK, PKT_ACK, PKT_DATA, PKT_FIN };

struct Packet {
    uint16_t type;
    uint32_t seq;
    uint32_t ack;
    uint32_t sack_mask; // 选择确认 数据包标记
    uint16_t length;
    uint16_t checksum;
    char data[MAX_PAYLOAD];
};

uint16_t calc_checksum(const Packet &pkt);
bool verify_checksum(const Packet &pkt);

#endif
