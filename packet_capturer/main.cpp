#include <iomanip>
#include <iostream>
#include <pcap.h>
#include <string>
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
    u_short TotalLen;     // 总长度
    u_short ID;           // 标识
    u_short Flag_Segment; // 标志 (3 bits) + 片偏移 (13 bits)
    u_char TTL;           // 生存时间
    u_char Protocol;      // 协议
    u_short Checksum;     // 首部校验和
    u_long SrcIP;         // 源地址
    u_long DstIP;         // 目的地址
} IPHeader_t;

// 包含帧首部和IP首部的数据包结构
typedef struct Data_t {
    FrameHeader_t FrameHeader;
    IPHeader_t IPHeader;
} Data_t;

class PacketCapturer {
  public:
    PacketCapturer() : handle(nullptr), alldevs(nullptr), device(nullptr) {}

    ~PacketCapturer() {
        if (handle) {
            pcap_close(handle);
        }
        if (alldevs) {
            pcap_freealldevs(alldevs);
        }
    }

    // 选择设备
    bool selectDevice() {
        char errbuf[PCAP_ERRBUF_SIZE];

        // 查找所有可用的网络设备
        if (pcap_findalldevs(&alldevs, errbuf) == -1) {
            std::cerr << "pacp查找设备出现错误: " << errbuf << std::endl;
            return false;
        }

        // 列出所有设备
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

    // 捕获数据包
    bool capturePackets(int packet_count = -1) {
        char errbuf[PCAP_ERRBUF_SIZE];

        // 打开设备进行捕获
        handle = pcap_open_live(device->name, BUFSIZ, 1, 1000, errbuf);
        if (handle == nullptr) {
            std::cerr << "无法打开设备 " << device->name << ": " << errbuf
                      << std::endl;
            return false;
        }

        // 捕获数据包
        // handle：由 pcap_open_live 返回的捕获句柄
        // packet_count：要捕获的数据包数量。设置为 0 表示无限制，直到手动停止。
        // packetHandler：处理每个捕获到的数据包的回调函数。
        // reinterpret_cast<u_char
        // *>(this)：将当前对象（当前PacketCapturer类的实例）的指针转换为 u_char
        // * 类型，并传递给回调函数
        if (pcap_loop(handle, packet_count, packetHandler,
                      reinterpret_cast<u_char *>(this)) < 0) {
            std::cerr << "数据包捕获出现错误: " << pcap_geterr(handle)
                      << std::endl;
            return false;
        }

        return true;
    }

    // 分析数据包
    void analyzePacket(const u_char *packet) {
        // 解析帧首部
        FrameHeader_t *frame_header = (FrameHeader_t *)packet;

        // 获取源MAC地址和目标MAC地址
        char src_mac[18];
        char dst_mac[18];
        snprintf(src_mac, sizeof(src_mac), "%02x:%02x:%02x:%02x:%02x:%02x",
                 frame_header->SrcMAC[0], frame_header->SrcMAC[1],
                 frame_header->SrcMAC[2], frame_header->SrcMAC[3],
                 frame_header->SrcMAC[4], frame_header->SrcMAC[5]);
        snprintf(dst_mac, sizeof(dst_mac), "%02x:%02x:%02x:%02x:%02x:%02x",
                 frame_header->DesMAC[0], frame_header->DesMAC[1],
                 frame_header->DesMAC[2], frame_header->DesMAC[3],
                 frame_header->DesMAC[4], frame_header->DesMAC[5]);

        // 获取类型/长度字段的值
        // 2048 (0x0800)：表示IPv4协议
        // 2054 (0x0806)：表示地址解析协议（ARP）
        // 34525 (0x86DD)：表示IPv6协议
        // 35020 (0x88CC)：表示链路层发现协议（LLDP）
        u_short frame_type_code = ntohs(frame_header->FrameType);
        std::string frame_type_label = "类型：";
        std::string frame_type;

        switch (frame_type_code) {
        case 0x0800:
            frame_type = "IPV4";
            break;
        case 0x0806:
            frame_type = "ARP";
            break;
        case 0x86DD:
            frame_type = "IPV6";
            break;
        case 0x88CC:
            frame_type = "LLDP";
            break;
        default:
            frame_type_label = "长度：";
            frame_type = std::to_string(frame_type_code);
            break;
        }

        // 输出源地址、目标地址和类型/长度字段的值，使用setw和left来格式化输出
        std::cout << std::left << std::setw(15) << "源MAC: " << std::setw(15)
                  << src_mac << " | " << std::left << std::setw(15)
                  << "目的MAC: " << std::setw(15) << dst_mac << " | "
                  << std::left << std::setw(15) << frame_type_label
                  << std::setw(15) << frame_type << std::endl;
    }

  private:
    // 数据包处理函数
    // 当有数据包到达时，libpcap库会调用此函数，函数的参数为：
    // 用户数据指针（u_char
    // *userData）（这里传入的是当前类PacketCapturer的实例），
    // 数据包头部指针（const struct pcap_pkthdr *packetHeader），
    // 数据包内容指针（const u_char *packetData）
    static void packetHandler(u_char *user, const struct pcap_pkthdr *header,
                              const u_char *packet) {
        (void)header; // 未使用数据包头部数据
        PacketCapturer *capturer = reinterpret_cast<PacketCapturer *>(user);
        capturer->analyzePacket(packet);
    }

    pcap_t *handle;                        // 用于捕获数据包的句柄
    pcap_if_t *alldevs;                    // 设备列表
    pcap_if_t *device;                     // 选择的设备
    std::vector<std::string> device_names; // 设备名称列表
};

int main() {
    SetConsoleOutputCP(CP_UTF8); // 设置控制台输出编码为UTF-8
    PacketCapturer capturer;

    if (!capturer.selectDevice()) {
        return 1;
    }

    if (!capturer.capturePackets()) {
        return 1;
    }

    return 0;
}