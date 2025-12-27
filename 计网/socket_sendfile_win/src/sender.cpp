#include "util.h"
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

    // 待传输的文件路径（以二进制方式发送）
    std::string file_path = "./input.jpg";

    // router IP 和端口
    const char *recv_ip = "127.0.0.1";
    uint16_t recv_port = 3455;

    // 创建 UDP 套接字
    // AF_INET：IPv4
    // SOCK_DGRAM：UDP
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "socket failed" << std::endl;
        WSACleanup();
        return -1;
    }

    // 接收端地址
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(recv_port); // 端口号转换为网络字节序

    // 将字符串形式的 IP 地址转换为网络字节序
    if (inet_pton(AF_INET, recv_ip, &addr.sin_addr) <= 0) {
        std::cerr << "IP addr wrong" << std::endl;
    }

    // 建立连接
    // 发送SYN包模拟连接建立
    Packet pkt{};
    pkt.type = PKT_SYN;                // 标记为 SYN 报文
    pkt.seq = 0;                       // 初始序列号
    pkt.checksum = calc_checksum(pkt); // 计算校验和

    // 发送 SYN 报文
    sendto(sock, (char *)&pkt, sizeof(pkt), 0, (sockaddr *)&addr, sizeof(addr));

    // 等待接收端返回 SYN-ACK
    recvfrom(sock, (char *)&pkt, sizeof(pkt), 0, nullptr, nullptr);
    std::cout << "connection build" << std::endl;

    // 数据发送
    // 以二进制方式打开待发送文件
    std::ifstream fin(file_path, std::ios::binary);

    // base：发送窗口起始序列号
    // nextseq：下一个可发送的数据包序列号
    uint32_t base = 1, nextseq = 1;

    // 拥塞窗口 cwnd，慢启动阈值 ssthresh
    // 初始进入慢启动阶段
    int cwnd = 1, ssthresh = WINDOW_SIZE;

    // 缓存已发送但尚未确认的数据包
    // key 为序列号，value 为对应的数据包
    std::map<uint32_t, Packet> buffer;

    // 主循环：直到文件全部发送且被确认
    while (true) {

        // 持续发送数据包直到填满窗口
        while (nextseq < base + cwnd && fin) {
            Packet data{};
            data.type = PKT_DATA; // 数据包类型
            data.seq = nextseq;   // 设置序列号

            // 从文件中读取MAX_PAYLOAD字节的数据
            fin.read(data.data, MAX_PAYLOAD);
            data.length = fin.gcount(); // 实际读取的字节数

            // 如果已经读到文件末尾，退出发送循环
            if (data.length == 0)
                break;

            // 计算数据包校验和
            data.checksum = calc_checksum(data);

            // 发送数据包
            sendto(sock, (char *)&data, sizeof(data), 0, (sockaddr *)&addr,
                   sizeof(addr));

            // 将该数据包缓存起来，等待 ACK
            buffer[nextseq] = data;

            // 序列号递增
            nextseq++;
        }

        // 等待 ACK
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(sock, &rfds);

        // 设置超时时间，用于判断是否发生丢包
        timeval tv{};
        tv.tv_sec = 0;
        tv.tv_usec = TIMEOUT_MS * 1000;

        // select 用于同时实现“等待 ACK + 超时检测”
        int ret = select(0, &rfds, nullptr, nullptr, &tv);

        if (ret > 0) {
            // 成功收到 ACK
            Packet ackpkt{};
            recvfrom(sock, (char *)&ackpkt, sizeof(ackpkt), 0, nullptr,
                     nullptr);

            // 如果是确认包
            if (ackpkt.type == PKT_ACK) {
                // 将发送窗口起点移动到最新确认位置之后
                base = ackpkt.ack + 1;

                // 拥塞控制逻辑
                if (cwnd < ssthresh)
                    cwnd++; // 慢启动阶段：指数增长
                else
                    cwnd += 1 / cwnd; // 拥塞避免阶段：线性增长
            }
        } else {
            // 超时未收到 ACK，认为发生丢包
            // 执行 Reno 超时处理
            ssthresh = cwnd / 2; // 更新慢启动阈值
            cwnd = 1;            // 回退到慢启动

            // 重传窗口中所有未确认的数据包
            for (auto &p : buffer) {
                sendto(sock, (char *)&p.second, sizeof(p.second), 0,
                       (sockaddr *)&addr, sizeof(addr));
            }
        }

        // 文件已读完，且所有数据都被确认，退出循环
        if (!fin && base == nextseq)
            break;
    }

    // 关闭连接
    // 发送 FIN 报文，通知接收端传输结束
    Packet finpkt{};
    finpkt.type = PKT_FIN;
    finpkt.checksum = calc_checksum(finpkt);

    sendto(sock, (char *)&finpkt, sizeof(finpkt), 0, (sockaddr *)&addr,
           sizeof(addr));

    // 关闭套接字
    closesocket(sock);
    WSACleanup();
    return 0;
}
