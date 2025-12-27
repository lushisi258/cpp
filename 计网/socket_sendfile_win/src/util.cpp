#include "util.h"

// 校验和计算
uint16_t calc_checksum(const Packet &pkt) {
    uint32_t sum = 0;
    const uint16_t *p = reinterpret_cast<const uint16_t *>(&pkt);
    size_t len = sizeof(Packet) / 2;

    for (size_t i = 0; i < len; i++) {
        sum += p[i];
        if (sum & 0x10000) {
            sum = (sum & 0xFFFF) + 1;
        }
    }
    return ~(sum & 0xFFFF);
}

bool verify_checksum(const Packet &pkt) { return calc_checksum(pkt) == 0; }
