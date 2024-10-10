#include <pcap.h>
#include <iostream>
#include <string>
#include <vector>
#include <iomanip> // 用于格式化输出的头文件
#include <winsock2.h> // Windows 特定的头文件
#include <ws2tcpip.h> // Windows 特定的头文件

#pragma comment(lib, "ws2_32.lib") // 链接 Winsock 库

#pragma pack(1)        // 进入字节对齐方式

// 帧首部结构
typedef struct FrameHeader_t {
    u_char DesMAC[6];  // 目的地址
    u_char SrcMAC[6];  // 源地址
    u_short FrameType; // 帧类型
} FrameHeader_t;

// IP首部结构
typedef struct IPHeader_t {
    u_char Ver_HLen;   // 版本 (4 bits) + 首部长度 (4 bits)
    u_char TOS;        // 服务类型
    u_short TotalLen;  // 总长度
    u_short ID;        // 标识
    u_short Flag_Segment; // 标志 (3 bits) + 片偏移 (13 bits)
    u_char TTL;        // 生存时间
    u_char Protocol;   // 协议
    u_short Checksum;  // 首部校验和
    u_long SrcIP;      // 源地址
    u_long DstIP;      // 目的地址
} IPHeader_t;

// 包含帧首部和IP首部的数据包结构
typedef struct Data_t {
    FrameHeader_t FrameHeader;
    IPHeader_t IPHeader;
} Data_t;

// 恢复默认对齐方式
#pragma pack()

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
        }
        else {
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
            std::cerr << "无法打开设备 " << device->name << ": "
                      << errbuf << std::endl;
            return false;
        }

        // 捕获数据包
        if (pcap_loop(handle, packet_count, packetHandler,
                      reinterpret_cast<u_char *>(this)) < 0) {
            std::cerr << "pcap_loop出现错误: " << pcap_geterr(handle)
                      << std::endl;
            return false;
        }

        return true;
    }

    // 分析数据包
    void analyzePacket(const struct pcap_pkthdr* header, const u_char* packet) {
        // 解析帧首部
        FrameHeader_t* frame_header = (FrameHeader_t*)packet;

        // 检查帧类型是否为IP
        if (ntohs(frame_header->FrameType) == 0x0800) {
            // 解析IP头
            IPHeader_t* ip_header = (IPHeader_t*)(packet + sizeof(FrameHeader_t));

            // 获取源地址和目标地址
            char src_ip[INET_ADDRSTRLEN];
            char dst_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(ip_header->SrcIP), src_ip, INET_ADDRSTRLEN);
            inet_ntop(AF_INET, &(ip_header->DstIP), dst_ip, INET_ADDRSTRLEN);

            // 输出源地址和目标地址，使用setw和left来格式化输出
            std::cout << std::left << std::setw(20) << "源IP: " << std::setw(20) << src_ip
                << " | "
                << std::left << std::setw(20) << "目标IP: " << std::setw(20) << dst_ip
                << std::endl;
        }
    }

  private:
	  // 数据包处理
    static void packetHandler(u_char *user, const struct pcap_pkthdr *header,
                              const u_char *packet) {
        PacketCapturer *capturer = reinterpret_cast<PacketCapturer *>(user);
        capturer->analyzePacket(header, packet);
    }

	pcap_t* handle; // 用于捕获数据包的句柄
	pcap_if_t* alldevs; // 设备列表
	pcap_if_t* device; // 选择的设备
	std::vector<std::string> device_names; // 设备名称列表
};

int main() {
    PacketCapturer capturer;

    if (!capturer.selectDevice()) {
        return 1;
    }

    if (!capturer.capturePackets()) {
        return 1;
    }

    return 0;
}