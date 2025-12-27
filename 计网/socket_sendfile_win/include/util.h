#ifndef UTIL_H
#define UTIL_H

#include <cstdint>
#include <cstring>

#define MAX_PAYLOAD 1000
#define WINDOW_SIZE 10
#define TIMEOUT_MS 500

enum PacketType {
    PKT_SYN = 1,
    PKT_SYN_ACK,
    PKT_ACK,
    PKT_DATA,
    PKT_FIN
};

struct Packet {
    uint16_t type;
    uint32_t seq;
    uint32_t ack;
    uint16_t length;
    uint16_t checksum;
    char data[MAX_PAYLOAD];
};

uint16_t calc_checksum(const Packet &pkt);
bool verify_checksum(const Packet &pkt);

#endif
