#include <winsock2.h>
#include <windows.h>
#include <iphlpapi.h>
#include <iostream>
#include <pcap/pcap.h>
#include <vector>
#include <cstring>
#include <chrono>

pcap_if_t *alldevs;
pcap_if_t *device;
std::vector<const char*> device_names;

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

// 选择设备
bool select_device() {
    char errbuf[PCAP_ERRBUF_SIZE];

    // 查找所有可用的网络设备
    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        std::cerr << "pcap查找设备出现错误: " << errbuf << std::endl;
        return false;
    }

    // 列出所有设备
    int i = 0;
    for (device = alldevs; device != nullptr; device = device->next) {
        std::cout << ++i << ": " << device->name;
        if (device->description) {
            std::cout << " (" << device->description << ")" << std::endl;
        }
        device_names.push_back(device->name);
    }

    if (i == 0) {
        std::cerr << "没有找到设备" << std::endl;
        return false;
    }

    // 让用户选择一个设备
    int choice;
    std::cout << "输入你要选择的设备的数字编号: ";
    std::cin >> choice;

    if (choice < 1 || choice > i) {
        std::cerr << "无效选择" << std::endl;
        return false;
    }

    // 选择用户指定的设备
    device = alldevs;
    for (int index = 1; index < choice; ++index) {
        device = device->next;
    }

    if (device != nullptr) {
        std::cout << "使用的设备是: " << device->name << std::endl;
        return true;
    } else {
        std::cerr << "设备指针为空。" << std::endl;
        return false;
    }
}

// 构造ARP请求包
void construct_arp_request(arp_packet &packet, const u_char *src_mac, const u_char *src_ip, const u_char *target_ip) {
    // 以太网帧头部
    memset(packet.dest_mac, 0xff, 6); // 广播地址
    memcpy(packet.src_mac, src_mac, 6);
    packet.eth_type = htons(0x0806); // ARP协议

    // ARP请求头部
    packet.hw_type = htons(1); // 以太网
    packet.proto_type = htons(0x0800); // IPv4
    packet.hw_size = 6; // MAC地址长度
    packet.proto_size = 4; // IP地址长度
    packet.opcode = htons(1); // ARP请求
    memcpy(packet.sender_mac, src_mac, 6);
    memcpy(packet.sender_ip, src_ip, 4);
    memset(packet.target_mac, 0x00, 6); // 目标MAC地址未知
    memcpy(packet.target_ip, target_ip, 4);
}

// 解析ARP响应包
bool parse_arp_response(const u_char *packet, const u_char *target_ip, u_char *target_mac) {
    const arp_packet *arp_resp = reinterpret_cast<const arp_packet*>(packet);

    // 检查以太网类型是否为ARP
    if (ntohs(arp_resp->eth_type) != 0x0806) {
        return false;
    }

    // 检查是否为ARP响应
    if (ntohs(arp_resp->opcode) != 2) {
        return false;
    }

    // 检查目标IP地址是否匹配
    if (memcmp(arp_resp->sender_ip, target_ip, 4) != 0) {
        return false;
    }

    // 提取目标MAC地址
    memcpy(target_mac, arp_resp->sender_mac, 6);
    return true;
}

// 获取MAC地址
void get_mac(pcap_t *adhandle, const u_char *src_mac, const u_char *src_ip, const u_char *target_ip) {
    // 构造ARP请求包
    arp_packet packet;
    construct_arp_request(packet, src_mac, src_ip, target_ip);

    // 发送ARP请求包
    if (pcap_sendpacket(adhandle, reinterpret_cast<const u_char*>(&packet), sizeof(packet)) != 0) {
        std::cerr << "发送ARP请求包失败: " << pcap_geterr(adhandle) << std::endl;
        pcap_close(adhandle);
        return;
    }

    // 捕获并解析ARP响应包
    struct pcap_pkthdr *header;
    const u_char *pkt_data;
    int res;
    u_char target_mac[6];
    // 设置超时时间为10秒
    auto start_time = std::chrono::steady_clock::now();
    auto timeout_duration = std::chrono::seconds(20);


    while ((res = pcap_next_ex(adhandle, &header, &pkt_data)) >= 0) {
        if (res == 0) {
            // 超时
            auto current_time = std::chrono::steady_clock::now();
            if (current_time - start_time >= timeout_duration) {
                std::cout << "超时" << std::endl;
                break;
            }
            continue;
        }

        if (parse_arp_response(pkt_data, target_ip, target_mac)) {
            std::cout << "目标MAC地址: ";
            for (int i = 0; i < 6; ++i) {
                printf("%02x", target_mac[i]);
                if (i < 5) {
                    std::cout << ":";
                }
            }
            std::cout << std::endl;
            break;
        }
    }

    if (res == -1) {
        std::cerr << "捕获数据包时出错: " << pcap_geterr(adhandle) << std::endl;
    }
}

int main() {
    // 选择设备
    if (!select_device()) {
        return -1;
    }

    // 打开设备
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *adhandle = pcap_open_live(device->name, 65536, 1, 1000, errbuf);
    if (adhandle == nullptr) {
        std::cerr << "无法打开适配器: " << device->name << " 错误: " << errbuf << std::endl;
        return -1;
    }

    // 示例MAC地址和IP地址
    u_char src_mac[6] = {0xE0, 0x0A, 0xF6, 0x96, 0x79, 0x21};
    u_char src_ip[4] = {10, 136, 164, 205};
    std::string target_ip_str;

    while (true) {
        std::cout << "目标IP地址: ";
        std::cin >> target_ip_str;

        if (target_ip_str == "exit") {
            break;
        }

        u_char target_ip[4];
        if (inet_pton(AF_INET, target_ip_str.c_str(), target_ip) != 1) {
            std::cerr << "无效的IP地址格式" << std::endl;
            continue;
        }

        get_mac(adhandle, src_mac, src_ip, target_ip);
    }

    // 关闭设备
    pcap_close(adhandle);
    return 0;
}