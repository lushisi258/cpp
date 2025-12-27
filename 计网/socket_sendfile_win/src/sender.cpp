#include "../include/util.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int main() {

    // 初始化WinSock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return -1;
    }

    std::string file_path = "./bin/input.jpg";

    const char *recv_ip = "127.0.0.1";
    uint16_t recv_port = 3455;

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "socket failed\n";
        WSACleanup();
        return -1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(recv_port);
    inet_pton(AF_INET, recv_ip, &addr.sin_addr);

    // 建立连接
    Packet syn{};
    syn.type = PKT_SYN;
    syn.seq = 0;
    syn.checksum = calc_checksum(syn);

    sendto(sock, (char *)&syn, sizeof(syn), 0, (sockaddr *)&addr, sizeof(addr));

    recvfrom(sock, (char *)&syn, sizeof(syn), 0, nullptr, nullptr);
    std::cout << "connection build\n";

    // 打开本地文件
    std::ifstream fin(file_path, std::ios::binary);
    if (!fin) {
        std::cerr << "open file failed\n";
        return -1;
    }

    uint32_t base = 1;
    uint32_t nextseq = 1;

    int cwnd = 1;               // 拥塞窗口
    int ssthresh = WINDOW_SIZE; // 慢启动阈值

    // 发送端数据包缓存
    struct SendEntry {
        Packet pkt;
        bool sacked;
    };
    std::map<uint32_t, SendEntry> buffer;

    // 配置计时器
    using clock_type = std::chrono::steady_clock;
    auto start_time = clock_type::now();
    auto end_time = clock_type::now();
    // 文件大小
    uint64_t total_bytes = 1857536;

    while (true) {

        // 发送数据直至填满窗口
        while (nextseq < base + cwnd && fin) {
            Packet data{};
            data.type = PKT_DATA;
            data.seq = nextseq;

            fin.read(data.data, MAX_PAYLOAD);
            data.length = fin.gcount();
            if (data.length == 0)
                break;

            data.checksum = calc_checksum(data);

            sendto(sock, (char *)&data, sizeof(data), 0, (sockaddr *)&addr,
                   sizeof(addr));

            // 将发送的数据包加入缓存，标记为未确认
            buffer[nextseq] = {data, false};
            nextseq++;
        }

        // 等待ACK
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(sock, &rfds);

        timeval tv{};
        tv.tv_sec = 0;
        tv.tv_usec = TIMEOUT_MS * 1000;

        int ret = select(0, &rfds, nullptr, nullptr, &tv);

        if (ret > 0) {
            Packet ack{};
            recvfrom(sock, (char *)&ack, sizeof(ack), 0, nullptr, nullptr);

            // 处理ACK
            if (ack.type == PKT_ACK && verify_checksum(ack) &&
                ack.ack >= base && ack.ack < nextseq) {

                base = ack.ack + 1;

                // 清理已确认数据
                for (auto it = buffer.begin(); it != buffer.end();) {
                    if (it->first < base)
                        it = buffer.erase(it);
                    else
                        ++it;
                }

                // 处理 SACK
                for (uint32_t i = 0; i < 32; i++) {
                    if (ack.sack_mask & (1u << i)) {
                        uint32_t seq = base + i;
                        if (buffer.count(seq)) {
                            buffer[seq].sacked = true;
                        }
                    }
                }

                // 拥塞控制
                if (cwnd < ssthresh) {
                    // 慢启动：每收到一个ack，cwnd + 1
                    cwnd++;
                } else {
                    // 拥塞避免：每RTT增长1
                    // ca_cnt记录收到的ack数量
                    static int ca_cnt = 0;
                    ca_cnt++;
                    if (ca_cnt >= cwnd) {
                        cwnd++;
                        ca_cnt = 0;
                    }
                }
            }
        } else {
            // 超时处理
            ssthresh = std::max(1, cwnd / 2);
            cwnd = 1;

            // 重传未确认包
            for (auto &it : buffer) {
                if (it.first >= base && !it.second.sacked) {
                    sendto(sock, (char *)&it.second.pkt, sizeof(it.second.pkt),
                           0, (sockaddr *)&addr, sizeof(addr));
                }
            }
        }

        // 发送完成后退出
        if (!fin && buffer.empty() && base == nextseq)
            break;
    }

    // 结束时间
    end_time = clock_type::now();

    // 发送FIN
    Packet finpkt{};
    finpkt.type = PKT_FIN;
    finpkt.checksum = calc_checksum(finpkt);

    sendto(sock, (char *)&finpkt, sizeof(finpkt), 0, (sockaddr *)&addr,
           sizeof(addr));

    // 传输时间和数据吞吐率
    std::chrono::duration<double> elapsed = end_time - start_time;
    double seconds = elapsed.count();

    double throughput_Bps = total_bytes / seconds;
    double throughput_Mbps = (total_bytes * 8.0) / (seconds * 1e6);

    std::cout << "Transfer time: " << seconds << " s\n";
    std::cout << "Total data: " << total_bytes << " bytes\n";
    std::cout << "Average throughput: " << throughput_Bps << " B/s ("
              << throughput_Mbps << " Mbps)\n";

    closesocket(sock);
    WSACleanup();
    return 0;
}
