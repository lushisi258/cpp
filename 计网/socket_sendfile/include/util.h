#include <arpa/inet.h>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sys/socket.h>
#include <thread>
#include <vector>

#define MYPROTO 222

#define TCP_FLAG_SYN 0x0002  // SYN标志位：第1位（二进制0000 0000 0000 0010）
#define TCP_FLAG_ACK 0x0010  // ACK标志位：第4位（二进制0000 0000 0001 0000）
#define TCP_FLAG_FIN 0x0001  // FIN标志位：第0位（二进制0000 0000 0000 0001）
#define TCP_FLAG_DATA 0x0004 // DATA标志位：第2位（二进制0000 0000 0000 0100）

enum class Status {
    CLOSED,
    LISTEN,
    SYN_SENT,
    SYN_RCVD,
    ESTABLISHED,
    FIN_WAIT,
    LAST_ACK
};

#pragma pack(push, 1)
struct MYHDR {
    uint32_t seq;
    uint32_t ack;
    uint16_t flags;
    uint16_t win;
    uint16_t checksum;
};
struct IP_HEADER {
    uint8_t version_ihl;
    uint8_t tos;
    uint16_t total_len;
    uint16_t id;
    uint16_t frag_off;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    uint32_t src_addr;
    uint32_t dst_addr;
};
#pragma pack(pop)

// 计算校验和
uint16_t checksum16(const uint8_t *data, size_t len);

std::vector<uint8_t> make_segment(uint32_t seq, uint32_t ack, uint16_t flags,
                                  uint16_t window,
                                  const uint8_t *payload = nullptr,
                                  size_t payload_len = 0);

int send_seg(uint8_t *seg, size_t seg_len);

int cmd_handle(std::string cmd);