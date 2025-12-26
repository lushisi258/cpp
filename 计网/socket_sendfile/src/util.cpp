#include "../include/util.h"

uint16_t checksum16(const uint8_t *data, size_t len) {
    uint32_t sum = 0;

    // 每16bit累加
    while (len > 1) {
        sum += *(const uint16_t *)data;
        data += 2;
        len -= 2;

        // 如果超过16位，回卷进位
        if (sum & 0xFFFF0000) {
            sum = (sum & 0xFFFF) + (sum >> 16);
        }
    }

    // 如果长度是奇数，多余的1字节补到高位
    if (len == 1) {
        sum += static_cast<uint16_t>(*data << 8);
    }

    // 回卷直到只剩16bit
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return static_cast<uint16_t>(~sum);
}

std::vector<uint8_t> make_segment(uint32_t seq, uint32_t ack, uint16_t flags,
                                  uint16_t window, const uint8_t *payload,
                                  size_t payload_len) {
    size_t total = sizeof(MYHDR) + payload_len;

    // 初始化数据段的内存空间
    std::vector<uint8_t> seg(total, 0);

    // seg.data() 返回seg在内存中的起始位置
    // reinterpret_cast<MYHDR *>则是表明该自seg起始位置的内存按照MYHDR的结构解释
    // 即将hdr存入seg的内存中
    MYHDR *hdr = reinterpret_cast<MYHDR *>(seg.data());
    hdr->seq = htonl(seq);
    hdr->ack = htonl(ack);
    hdr->flags = htons(flags);
    hdr->win = htons(window);
    hdr->checksum = 0;

    if (payload && payload_len > 0) {
        memcpy(seg.data() + sizeof(MYHDR), payload, payload_len);
    }

    hdr->checksum = checksum16(seg.data(), total);
    return seg;
}

int send_seg(uint8_t *seg, size_t seg_len) { return 0; }
