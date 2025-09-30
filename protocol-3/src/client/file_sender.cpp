#include "file_sender.h"
#include <condition_variable>
#include <map>
#include <mutex>
#include <thread>
#include <unordered_set>

// 将套接字设置为非阻塞模式
void set_nonblocking(int sockfd) {
    u_long mode = 1; // 1 表示启用非阻塞模式
    if (ioctlsocket(sockfd, FIONBIO, &mode) != 0) {
        std::cerr << "设置非阻塞模式失败，错误代码：" << WSAGetLastError()
                  << std::endl;
    }
}

void send_file(int sockfd, sockaddr_in &server_addr, int addr_len,
               const char *filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "打开文件失败：" << filename << std::endl;
        return;
    }

    const int TIMEOUT = 2000; // 超时时间（毫秒）
    Packet packet;
    int seq_num = 0, base = 0;
    int cwnd = 1, ssthresh = 128;
    std::map<int, Packet> window;
    std::map<int, std::chrono::time_point<std::chrono::steady_clock>> timers;
    std::unordered_set<int> acks_received;
    std::mutex mtx;
    std::condition_variable cv;
    bool transfer_done = false;

    set_nonblocking(sockfd);

    auto send_thread = [&]() {
        while (true) {
            std::unique_lock<std::mutex> lock(mtx);

            if (transfer_done && window.empty()) {
                break;
            }

            while ((int)window.size() < cwnd &&
                   file.read(packet.data, sizeof(packet.data))) {
                packet.seq_num = seq_num;
                packet.checksum = calculate_checksum(packet);
                window[seq_num] = packet;
                timers[seq_num] = std::chrono::steady_clock::now();

                sendto(sockfd, (char *)&packet, sizeof(packet), 0,
                       (struct sockaddr *)&server_addr, addr_len);
                std::cout << "发送包：" << seq_num << " 校验和："
                          << packet.checksum << std::endl;
                seq_num++;
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }

            auto now = std::chrono::steady_clock::now();
            for (auto it = timers.begin(); it != timers.end();) {
                if (std::chrono::duration_cast<std::chrono::milliseconds>(
                        now - it->second)
                        .count() > TIMEOUT) {
                    int seq = it->first;
                    if (acks_received.find(seq) == acks_received.end()) {
                        sendto(sockfd, (char *)&window[seq], sizeof(packet), 0,
                               (struct sockaddr *)&server_addr, addr_len);
                        std::cout << "超时，重发包：" << seq << std::endl;

                        ssthresh = std::max(cwnd / 2, 1);
                        cwnd = 1;
                        it->second = now; // 重置定时器
                    }
                }
                ++it;
            }

            if (file.eof() && window.empty()) {
                transfer_done = true;
                cv.notify_all();
                break;
            }

            cv.wait_for(lock, std::chrono::milliseconds(10));
        }
    };

    auto recv_thread = [&]() {
        Packet ack_packet;
        while (true) {
            {
                std::unique_lock<std::mutex> lock(mtx);
                if (transfer_done && window.empty()) {
                    std::cout << "文件传输完成。" << std::endl;
                    break;
                }
            }

            int recv_len = recvfrom(
                sockfd, (char *)&ack_packet, sizeof(ack_packet), 0,
                (struct sockaddr *)&server_addr, (socklen_t *)&addr_len);
            if (recv_len > 0) {
                std::unique_lock<std::mutex> lock(mtx);
                if (acks_received.find(ack_packet.seq_num) == acks_received.end()) {
                    std::cout << "收到ACK：" << ack_packet.seq_num << std::endl;
                    acks_received.insert(ack_packet.seq_num);
                    window.erase(ack_packet.seq_num);
                    timers.erase(ack_packet.seq_num);

                    if (cwnd < ssthresh) {
                        cwnd *= 2; // 慢启动阶段
                    } else {    
                        cwnd += 1; // 拥塞避免阶段
                    }
                    cv.notify_all();
                }
            }
        }
    };

    std::thread sender(send_thread);
    std::thread receiver(recv_thread);

    sender.join();
    receiver.join();

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