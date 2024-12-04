#include "file_sender.h"

void send_file(SOCKET sockfd, sockaddr_in &server_addr, int addr_len,
               const char *filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "打开文件失败：" << filename << std::endl;
        return;
    }

    Packet packet, ack_packet;
    int seq_num = 0;
    while (file.read(packet.data, sizeof(packet.data)) || file.gcount() > 0) {
        packet.seq_num = seq_num;
        packet.checksum = calculate_checksum(packet);
        int retries = 0;

        // 使用 sendto 函数发送数据包，并设置 2 秒的接收超时
        // 然后使用 recvfrom 函数等待接收确认包
        // 如果收到的确认包序列号与当前数据包序列号匹配，表示确认成功，退出循环并发送下一个数据包
        // 如果超时或确认包序列号不匹配，重试发送数据包，最多重试 5 次
        while (true) {
            sendto(sockfd, (char *)&packet, sizeof(packet), 0,
                   (struct sockaddr *)&server_addr, addr_len);
            std::cout << "发送第" << seq_num << "个包，其校验和为"
                      << packet.checksum << std::endl;
                      
            DWORD timeout = 2000; // 2 秒超时
            setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                       sizeof(timeout));

            if (recvfrom(sockfd, (char *)&ack_packet, sizeof(ack_packet), 0,
                         (struct sockaddr *)&server_addr, &addr_len) > 0) {
                if (ack_packet.seq_num == seq_num) {
                    std::cout << "收到了第" << seq_num << "个包的ACK"
                              << std::endl;
                    seq_num++;
                    break;
                }
            }

            retries++;
            if (retries >= 5) {
                std::cerr << "超过最大重试次数" << seq_num << std::endl;
                return;
            }
            std::cout << "超时，重新发送" << seq_num << std::endl;
        }
    }
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