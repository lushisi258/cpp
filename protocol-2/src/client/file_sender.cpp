#include "file_sender.h"
#include <condition_variable>
#include <map>
#include <mutex>
#include <thread>

void send_file(SOCKET sockfd, sockaddr_in &server_addr, int addr_len,
               const char *filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "打开文件失败：" << filename << std::endl;
        return;
    }

    const int WINDOW_SIZE = 24; // 窗口大小
    const int TIMEOUT = 1000;   // 超时时间，单位为毫秒
    Packet packet;
    int seq_num = 0;
    int base = 0;
    std::map<int, int> acks;      // 快速重传的 ACK 计数器
    std::map<int, Packet> window; // 用于存储窗口内的数据包
    std::map<int, std::chrono::time_point<std::chrono::steady_clock>>
        timers; // 定时器
    std::mutex mtx;
    std::condition_variable cv;
    bool transfer_done = false;

    // 发送线程
    auto send_thread = [&]() {
        while (true) {
            std::unique_lock<std::mutex> lock(mtx);

            // 如果所有数据已经发送完毕并且窗口为空，则退出
            if (transfer_done && window.empty()) {
                break;
            }

            // 如果窗口未满或者还有数据可读，则继续读取发送数据
            while (window.size() < WINDOW_SIZE &&
                   file.read(packet.data, sizeof(packet.data))) {
                packet.seq_num = seq_num;
                packet.checksum = calculate_checksum(packet);
                window.insert({seq_num, packet}); // 将数据包插入窗口
                timers[seq_num] =
                    std::chrono::steady_clock::now(); // 设置定时器
                sendto(sockfd, (char *)&packet, sizeof(packet), 0,
                       (struct sockaddr *)&server_addr, addr_len);
                std::cout << "发送包" << seq_num << " 校验和" << packet.checksum
                          << std::endl;
                seq_num++;
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }

            // 检查超时重传
            auto now = std::chrono::steady_clock::now();
            for (auto it = timers.begin(); it != timers.end();) {
                if (std::chrono::duration_cast<std::chrono::milliseconds>(
                        now - it->second)
                        .count() > TIMEOUT) {
                    int seq = it->first;
                    sendto(sockfd, (char *)&window[seq], sizeof(packet), 0,
                           (struct sockaddr *)&server_addr, addr_len);
                    std::cout << "超时，重发包" << seq << std::endl;
                    it->second = now; // 重置定时器
                }
                ++it;
            }

            if (file.eof() && window.empty()) {
                transfer_done = true;
                break;
            }
        }
    };

    auto recv_thread = [&]() {
        Packet ack_packet;
        while (true) {
            // 检查是否应该退出循环
            if (transfer_done && window.empty()) {
                std::cout << "文件传输完毕" << std::endl;
                break;
            } else {
                fd_set readfds; // 定义文件描述符集合
                struct timeval tv; // 定义时间结构体，用于设置超时时间
                int retval;        // 定义返回值变量

                // 初始化文件描述符集合，将所有位清零
                FD_ZERO(&readfds);
                // 将sockfd加入到文件描述符集合中
                FD_SET(sockfd, &readfds);

                // 设置超时时间为1秒
                tv.tv_sec = 1;  // 秒
                tv.tv_usec = 0; // 微秒

                // 使用select函数等待sockfd上的数据到达或超时
                retval = select(sockfd + 1, &readfds, NULL, NULL, &tv);

                if (retval == -1) {
                    // 如果select函数返回-1，表示发生错误
                    perror("select()");
                    break;
                } else if (retval == 0) {
                    // 如果select函数返回0，表示超时，没有数据到达
                    continue;
                }
            }
            if (recvfrom(sockfd, (char *)&ack_packet, sizeof(ack_packet), 0,
                         (struct sockaddr *)&server_addr, &addr_len)) {
                std::unique_lock<std::mutex> lock(mtx);
                if (acks[ack_packet.seq_num]++ >= 3) {
                    // 如果收到 3 次重复的 ACK，则进行快速重传
                    int seq = ack_packet.seq_num;
                    sendto(sockfd, (char *)&window[seq + 1], sizeof(packet), 0,
                           (struct sockaddr *)&server_addr, addr_len);
                    std::cout << "快速重传包" << seq + 1 << std::endl;
                    acks[seq] = 0;
                }
                // 如果 ACK 在窗口范围内，则滑动窗口
                if (ack_packet.seq_num >= base) {
                    std::cout << "收到合法ACK包" << ack_packet.seq_num
                              << std::endl;

                    // 滑动窗口将已经确认的数据包移除
                    for (int i = base; i <= ack_packet.seq_num; i++) {
                        window.erase(i);
                        timers.erase(i);
                    }

                    // 更新 base
                    std::cout << "更新base为" << ack_packet.seq_num + 1
                              << std::endl;
                    base = ack_packet.seq_num + 1;

                } else {
                    std::cout << "忽略ACK包" << ack_packet.seq_num << std::endl;
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