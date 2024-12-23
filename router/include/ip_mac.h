#include <chrono>
#include <cstring>
#include <iostream>
#include <iphlpapi.h>
#include <pcap.h>
#include <vector>
#include <windows.h>
#include <winsock2.h>

// ARP请求包的结构
struct arp_packet {
    // 以太网帧头部
    u_char dest_mac[6];
    u_char src_mac[6];
    u_short eth_type;

    // ARP请求头部
    u_short hw_type;
    u_short proto_type;
    u_char hw_size;
    u_char proto_size;
    u_short opcode;
    u_char sender_mac[6];
    u_char sender_ip[4];
    u_char target_mac[6];
    u_char target_ip[4];
};
