#include "../include/util.h"
#include <fstream>
#include <iostream>
#include <map>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int main() {

    // WinSock 初始化
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return -1;
    }

    // 接收后文件保存路径
    std::string file_saved = "./bin/saved.jpg";

    // 监听端口号
    uint16_t port = 3456;

    // 创建 UDP 套接字
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "socket create failed" << std::endl;
        WSACleanup();
        return -1;
    }

    // 本地地址和客户端地址结构体
    sockaddr_in addr{}, client{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);       // 端口号转为网络字节序
    addr.sin_addr.s_addr = INADDR_ANY; // 接收来自任意 IP 的数据

    // 将套接字绑定到指定端口
    if (bind(sock, (sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "bind failed" << std::endl;
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    // 客户端地址长度
    int len = sizeof(client);

    // 打开文件，用于保存接收到的二进制数据
    std::ofstream fout(file_saved, std::ios::binary);

    // 期望接收到的下一个数据包序列号
    uint32_t expected = 1;

    // 缓存乱序到达的数据包
    // key 为序列号，value 为对应的数据包
    std::map<uint32_t, Packet> buffer;

    // 主循环，持续接收发送端的数据和控制报文
    while (true) {

        Packet pkt{};

        // 接收来自发送端的数据包或控制包
        int ret = recvfrom(sock, (char *)&pkt, sizeof(pkt), 0,
                           (sockaddr *)&client, &len);
        if (ret <= 0)
            continue;

        // 校验和错误，直接丢弃该包
        // 通过不返回 ACK 触发发送端超时重传
        if (!verify_checksum(pkt))
            continue;

        // 建立连接
        if (pkt.type == PKT_SYN) {

            // 收到 SYN 报文，回复 SYN-ACK
            Packet synack{};
            synack.type = PKT_SYN_ACK;
            synack.checksum = calc_checksum(synack);

            sendto(sock, (char *)&synack, sizeof(synack), 0,
                   (sockaddr *)&client, len);
        }

        // 处理数据包
        else if (pkt.type == PKT_DATA) {

            // 如果是期望的序列号，说明按序到达
            if (pkt.seq == expected) {

                std::cout << "recv package " << expected << std::endl;
                // 直接写入文件
                fout.write(pkt.data, pkt.length);
                expected++;

                // 检查缓存中是否存在后续连续的数据包
                // 如果有，则按序写入文件
                while (buffer.count(expected)) {
                    fout.write(buffer[expected].data, buffer[expected].length);
                    buffer.erase(expected);
                    expected++;
                }
            }

            // 如果序列号大于期望值，说明乱序到达
            // 先缓存，等待前面的数据包
            else if (pkt.seq > expected) {
                buffer[pkt.seq] = pkt;
            }

            // 向发送端返回 ACK
            // ack 表示当前已连续正确接收的最大序列号
            Packet ack{};
            ack.type = PKT_ACK;
            ack.ack = expected - 1;
            ack.sack_mask = 0;

            // 从 expected 开始，回传已经收到的数据包信息
            for (uint32_t i = 0; i < 32; i++) {
                uint32_t seq = expected + i;
                if (buffer.count(seq)) {
                    ack.sack_mask |= (1u << i);
                }
            }

            ack.checksum = calc_checksum(ack);

            sendto(sock, (char *)&ack, sizeof(ack), 0, (sockaddr *)&client,
                   len);

        }

        // 关闭连接
        else if (pkt.type == PKT_FIN) {

            // 收到 FIN，表示发送端已完成数据发送
            std::cout << "send finished" << std::endl;
            break;
        }
    }

    // 关闭套接字，释放资源
    closesocket(sock);
    WSACleanup();
    return 0;
}
