#include "file_sender.h"
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

void send_file(SOCKET sockfd, sockaddr_in &server_addr, int addr_len,
               const char *filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "打开文件失败：" << filename << std::endl;
        return;
    }

    const int WINDOW_SIZE = 24; // 定义窗口大小
    Packet packet;
    int seq_num = 0;
    int base = 0;
    std::queue<Packet> window;
    std::mutex mtx;
    std::condition_variable cv;
    bool ack_received = false;

    auto send_thread = [&]() {
        while (file.read(packet.data, sizeof(packet.data)) ||
               file.gcount() > 0 || !window.empty()) {
            std::unique_lock<std::mutex> lock(mtx);
            if (file.gcount() > 0) {
                packet.seq_num = seq_num;
                packet.checksum = calculate_checksum(packet);
                window.push(packet);
                sendto(sockfd, (char *)&packet, sizeof(packet), 0,
                       (struct sockaddr *)&server_addr, addr_len);
                std::cout << "发送第" << seq_num << "个包，其校验和为"
                          << packet.checksum << std::endl;
                seq_num++;
            }

            if (window.size() >= WINDOW_SIZE) {
                cv.wait(lock, [&]() { return ack_received; });
                ack_received = false;
            }
        }
    };

    auto recv_thread = [&]() {
        Packet ack_packet;
        while (true) {
            if (recvfrom(sockfd, (char *)&ack_packet, sizeof(ack_packet), 0,
                         (struct sockaddr *)&server_addr, &addr_len) > 0) {
                std::unique_lock<std::mutex> lock(mtx);
                if (ack_packet.seq_num >= base) {
                    std::cout << "收到了第" << ack_packet.seq_num << "个包的ACK"
                              << std::endl;
                    while (!window.empty() &&
                           window.front().seq_num <= ack_packet.seq_num) {
                        window.pop();
                        base++;
                    }
                    ack_received = true;
                    cv.notify_one();
                }
            } else {
                std::cout << "超时，重新发送窗口中的数据包" << std::endl;
                std::queue<Packet> temp_window = window;
                while (!temp_window.empty()) {
                    Packet resend_packet = temp_window.front();
                    sendto(sockfd, (char *)&resend_packet,
                           sizeof(resend_packet), 0,
                           (struct sockaddr *)&server_addr, addr_len);
                    std::cout << "重新发送第" << resend_packet.seq_num
                              << "个包，其校验和为" << resend_packet.checksum
                              << std::endl;
                    temp_window.pop();
                }
            }
        }
    };

    std::thread sender(send_thread);
    std::thread receiver(recv_thread);

    sender.join();
    receiver.detach();

    file.close();
}

void send_file_name(SOCKET sockfd, sockaddr_in &server_addr, int addr_len,
                    const char *filename) {
    sendto(sockfd, filename, strlen(filename), 0,
           (struct sockaddr *)&server_addr, addr_len);
    std::cout << "发送文件名：" << filename << std::endl;
}

bool client_establish_connection(SOCKET sockfd, sockaddr_in &server_addr,
                                 int addr_len) {
    std::cout << "建立连接中" << std::endl;
    Packet packet, ack_packet;
    packet.seq_num = 0;
    strcpy(packet.data, "SYN"); // 标记请求建立连接
    packet.checksum = calculate_checksum(packet);
    sendto(sockfd, (char *)&packet, sizeof(packet), 0,
           (struct sockaddr *)&server_addr, addr_len); // 发送 SYN 请求
    std::cout << "发送了 SYN 请求" << std::endl;

    // 等待服务器的响应（SYN-ACK）
    recvfrom(sockfd, (char *)&ack_packet, sizeof(ack_packet), 0,
             (struct sockaddr *)&server_addr, &addr_len);
    if (strcmp(ack_packet.data, "SYN-ACK") == 0) { // 判断是否是 SYN-ACK 响应
        packet.seq_num = 1;
        strcpy(packet.data, "ACK"); // 发送 ACK 确认消息
        packet.checksum = calculate_checksum(packet);
        sendto(sockfd, (char *)&packet, sizeof(packet), 0,
               (struct sockaddr *)&server_addr, addr_len); // 发送 ACK
        std::cout << "连接建立" << std::endl;
        return true;
    }
    return false;
}

// 客户端断开连接：发送 FIN 请求，等待服务器的 ACK 确认消息
int close_connection(SOCKET sockfd, sockaddr_in &server_addr, int addr_len) {
    Packet packet, ack_packet;
    packet.seq_num = 1;
    strcpy(packet.data, "FIN");
    packet.checksum = calculate_checksum(packet);
    sendto(sockfd, (char *)&packet, sizeof(packet), 0,
           (struct sockaddr *)&server_addr, addr_len);
    std::cout << "发送 FIN 包" << std::endl;

    recvfrom(sockfd, (char *)&ack_packet, sizeof(ack_packet), 0,
             (struct sockaddr *)&server_addr, &addr_len);
    if (strcmp(ack_packet.data, "ACK") == 0) {
        std::cout << "收到 ACK 包，关闭连接" << std::endl;
    }

    return 0;
}