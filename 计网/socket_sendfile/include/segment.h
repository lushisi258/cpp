#include <cstdint>
#include <iostream>

#pragma pack(push, 1)
struct TCP_SEGMENT {
    IP_HEADER ip_header;
    TCP_HEADER tcp_header;
    APPLICATION_DATA data;
};

struct IP_HEADER {
    uint8_t version_ihl; // 版本 (4 bits) 和 IHL (4 bits)
    uint8_t tos;         // 服务类型
    uint16_t total_len;  // 总长度
    uint16_t id;         // 标识符
    uint16_t frag_off;   // 标志和分片偏移
    uint8_t ttl;         // 生存时间
    uint8_t protocol;    // 协议字段
    uint16_t checksum;   // 头部校验和
    uint32_t src_addr;   // 源 IP 地址
    uint32_t dst_addr;   // 目的 IP 地址
};

struct TCP_HEADER {
    uint16_t src_port;      // 源端口
    uint16_t dst_port;      // 目的端口
    uint32_t seq_num;       // 序号
    uint32_t ack_num;       // 确认号
    uint16_t off_res_flags; // 数据偏移/保留位/标志位
    uint16_t window_size;   // 窗口大小
    uint16_t checksum;      // 校验和
    uint16_t urg_ptr;       // 紧急指针
};

struct APPLICATION_DATA {};
#pragma pack(pop)