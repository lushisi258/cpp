#include <cstring>
#include <iostream>
#include <pcap.h>
#include <string>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>

// 初始化 Winsock
bool initializeWinsock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed: " << WSAGetLastError() << std::endl;
        return false;
    }
    return true;
}

const char *inet_ntop_win(int af, const void *src, char *dst, socklen_t size) {
    if (af == AF_INET) {
        struct sockaddr_in in{};
        in.sin_family = AF_INET;
        memcpy(&in.sin_addr, src, sizeof(struct in_addr));
        if (getnameinfo(reinterpret_cast<struct sockaddr *>(&in), sizeof(in), dst,
                        size, nullptr, 0, NI_NUMERICHOST) != 0) {
            return nullptr;
        }
        return dst;
    }
    return nullptr;
}

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

// 路由表条目
struct RouteEntry {
    std::string destination;
    std::string netmask;
    std::string nextHop;
    std::string iface;
};

class SimpleRouter {
  public:
    SimpleRouter(const std::vector<RouteEntry> &routeTable)
        : routeTable(routeTable), alldevs(nullptr), device(nullptr) {}

    ~SimpleRouter() {
        if (alldevs) {
            pcap_freealldevs(alldevs);
        }
    }

    bool selectDevice() {
        char errbuf[PCAP_ERRBUF_SIZE];

        if (pcap_findalldevs(&alldevs, errbuf) == -1) {
            std::cerr << "pcap查找设备出现错误: " << errbuf << std::endl;
            return false;
        }

        int i = 0;
        for (device = alldevs; device != nullptr; device = device->next) {
            std::cout << ++i << ": " << device->name;
            if (device->description) {
                std::cout << " (" << device->description << ")";
            }
            std::cout << std::endl;
            device_names.push_back(device->name);
        }

        if (i == 0) {
            std::cerr << "没有找到设备" << std::endl;
            return false;
        }

        int choice;
        std::cout << "输入你要选择的设备的数字编号: ";
        std::cin >> choice;

        if (choice < 1 || choice > i) {
            std::cerr << "无效选择" << std::endl;
            return false;
        }

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

    void start() {
        if (!device) {
            std::cerr << "未选择设备，无法开始。" << std::endl;
            return;
        }

        char errbuf[PCAP_ERRBUF_SIZE];
        pcap_t *handle = pcap_open_live(device->name, BUFSIZ, 1, 1000, errbuf);

        if (!handle) {
            std::cerr << "打开设备失败: " << errbuf << std::endl;
            return;
        }

        std::cout << "开始监听设备: " << device->name << std::endl;

        pcap_loop(handle, -1, packetHandler, reinterpret_cast<u_char *>(this));
        pcap_close(handle);
    }

  private:
    std::vector<RouteEntry> routeTable;
    pcap_if_t *alldevs;
    pcap_if_t *device;
    std::vector<std::string> device_names;

    static void packetHandler(u_char *userData,
                              const struct pcap_pkthdr *pkthdr,
                              const u_char *packet) {
        auto *router = reinterpret_cast<SimpleRouter *>(userData);
        router->processPacket(packet, pkthdr->len);
    }

    void processPacket(const u_char *packet, int length) {
        if (length < sizeof(FrameHeader_t) + sizeof(IPHeader_t)) {
            std::cerr << "数据包太小，忽略！" << std::endl;
            return;
        }

        auto *frameHeader = reinterpret_cast<const FrameHeader_t *>(packet);
        auto *ipHeader = reinterpret_cast<const IPHeader_t *>(
            packet + sizeof(FrameHeader_t));

        if (ntohs(frameHeader->FrameType) != 0x0800) {
            std::cout << "非IP包，忽略。" << std::endl;
            return;
        }

        char srcIP[INET_ADDRSTRLEN], dstIP[INET_ADDRSTRLEN];
        inet_ntop_win(AF_INET, &ipHeader->SrcIP, srcIP, sizeof(srcIP));
        inet_ntop_win(AF_INET, &ipHeader->DstIP, dstIP, sizeof(dstIP));

        std::cout << "IP包: " << srcIP << " -> " << dstIP << std::endl;

        if (ipHeader->TTL <= 1) {
            std::cerr << "TTL已过期，丢弃包。" << std::endl;
            return;
        }

        std::string nextHop, iface;
        if (!findRoute(dstIP, nextHop, iface)) {
            std::cerr << "没有找到到 " << dstIP << " 的路由。" << std::endl;
            return;
        }

        std::cout << "路由到 " << nextHop << " 通过接口 " << iface << std::endl;
    }

    bool findRoute(const std::string &dstIP, std::string &nextHop,
                   std::string &iface) {
        for (const auto &entry : routeTable) {
            if (matchIP(dstIP, entry.destination, entry.netmask)) {
                nextHop = entry.nextHop.empty() ? dstIP : entry.nextHop;
                iface = entry.iface;
                return true;
            }
        }
        return false;
    }

    bool matchIP(const std::string &ip, const std::string &network,
                 const std::string &netmask) {
        struct in_addr ipAddr, networkAddr, maskAddr;
        inet_pton(AF_INET, ip.c_str(), &ipAddr);
        inet_pton(AF_INET, network.c_str(), &networkAddr);
        inet_pton(AF_INET, netmask.c_str(), &maskAddr);

        return (ipAddr.s_addr & maskAddr.s_addr) ==
               (networkAddr.s_addr & maskAddr.s_addr);
    }
};

int main() {
    // 设置控制台输出代码页为 UTF-8
    SetConsoleOutputCP(CP_UTF8);

    if (!initializeWinsock()) {
        return 1;
    }
    std::vector<RouteEntry> routeTable = {
        {"192.168.188.133", "255.255.255.0", "192.168.188.2", "eth0"},
        {"10.0.0.0", "255.0.0.0", "", "eth1"}};

    SimpleRouter router(routeTable);

    if (router.selectDevice()) {
        router.start();
    }

    WSACleanup();
    return 0;
}
