#ifndef ROUTER_SELECT_H
#define ROUTER_SELECT_H

#include "ip_mac.h"
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <mutex>
#include <pcap.h>
#include <queue>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// 帧首部结构
typedef struct FrameHeader_t {
    u_char DesMAC[6];  // 目的地址
    u_char SrcMAC[6];  // 源地址
    u_short FrameType; // 帧类型或长度
} FrameHeader_t;

// IP首部结构
typedef struct IPHeader_t {
    u_char Ver_HLen;      // 版本 (4 bits) + 首部长度 (4 bits)
    u_char TOS;           // 服务类型
    u_short ID;           // 标识
    u_short Flag_Segment; // 标志 (3 bits) + 片偏移 (13 bits)
    u_char TTL;           // 生存时间
    u_char Protocol;      // 协议
    u_short Checksum;     // 首部校验和
    u_long SrcIP;         // 源地址
    u_long DstIP;         // 目的地址
} IPHeader_t;

// 数据包结构
typedef struct Packet_t {
    FrameHeader_t FrameHeader; // 帧首部
    IPHeader_t IPHeader;       // IP首部
    u_char Payload[1500];      // 数据
} Packet_t;

// 路由表
struct RouteEntry {
    std::string destination;
    std::string netmask;
    std::string nextHop;
    std::string iface;
};

class Router {
  public:
    Router();
    void add_route(const std::string &destination, const std::string &gateway,
                   const std::string &netmask, const std::string &iface);
    RouteEntry *find_route(const std::string &destination);
    int forward();
    void start();
    void stop();
    ~Router();

  private:
    int select_device();
    int capture_packets(int packet_coun);
    // 使用静态成员函数作为回调函数
    // pcap_loop函数期望的回调函数类型是pcap_handler
    // 而成员函数有一个隐含的this指针参数，不能直接作为回调函数传递
    static void packet_handler(u_char *user, const struct pcap_pkthdr *header,
                               const u_char *packet);
    // 对数据包进行处理
    void handle_packet(const u_char *packet);

    void forward_packets(const u_char *packet);

    pcap_t *handle = nullptr;                     // 捕获的数据包的句柄
    pcap_if_t *alldevs = nullptr;                 // 设备列表
    pcap_if_t *device = nullptr;                  // 选择的设备
    std::vector<std::string> device_names;        // 设备名称列表
    std::vector<u_long> device_ip;                // 设备的IP
    std::queue<std::vector<u_char>> packet_queue; // 数据包队列
    std::vector<RouteEntry> routingTable;

    std::thread receive_thread;
    std::thread forward_thread;
    std::mutex mtx;
    std::condition_variable cv;
    bool running = false;
};

#endif // ROUTER_SELECT_H