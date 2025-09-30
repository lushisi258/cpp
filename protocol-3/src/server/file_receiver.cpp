#include "file_receiver.h"
#include <map>

void receive_file(SOCKET sockfd, sockaddr_in &client_addr, int addr_len, const char *output_filename) {
    // 创建文件
    std::ofstream file(output_filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to create file: " << output_filename << std::endl;
        return;
    }

    // 初始化
    Packet packet, ack_packet;
    int expected_seq_num = 0;
    std::map<int, Packet> buffer; // 缓冲区

    while (true) {
        int bytes = recvfrom(sockfd, (char *)&packet, sizeof(packet), 0, (struct sockaddr *)&client_addr, &addr_len);
        if (bytes <= 0)
            continue;

        // 检查是否是 FIN 包
        if (strcmp(packet.data, "FIN") == 0) {
            std::cout << "收到FIN包" << std::endl;
            break;
        }

        int checksum = calculate_checksum(packet);
        if (checksum == packet.checksum) {
            if (packet.seq_num == expected_seq_num) {
                // 写入文件
                file.write(packet.data, bytes - offsetof(Packet, data));
                std::cout << "收到包" << packet.seq_num << std::endl;
                expected_seq_num++;

                // 检查缓冲区中是否有后续的包
                while (buffer.find(expected_seq_num) != buffer.end()) {
                    Packet &buffered_packet = buffer[expected_seq_num];
                    file.write(buffered_packet.data, sizeof(buffered_packet.data));
                    std::cout << "写入包" << expected_seq_num << std::endl;
                    buffer.erase(expected_seq_num);
                    expected_seq_num++;
                }
            } else if (packet.seq_num > expected_seq_num) {
                // 存储到缓冲区
                buffer[packet.seq_num] = packet;
                std::cout << "收到包" << packet.seq_num << " 预期的包为" << expected_seq_num << std::endl;
            }

            // 收到包后发送 ACK
            ack_packet.seq_num = packet.seq_num;
            sendto(sockfd, (char *)&ack_packet, sizeof(ack_packet), 0, (struct sockaddr *)&client_addr, addr_len);
            std::cout << "发送ACK包" << ack_packet.seq_num << std::endl;

        } else {
            std::cerr << "校验和出错，包" << packet.seq_num << "丢弃" << std::endl;
        }
    }

    std::cout << "文件传输完毕" << std::endl;
    file.close();
}


char *get_file_name(sockaddr_in &client_addr, SOCKET sockfd) {
    static char file_name[256];
    int addr_len = sizeof(client_addr);
    int bytes = recvfrom(sockfd, file_name, sizeof(file_name) - 1, 0,
                         (struct sockaddr *)&client_addr, &addr_len);
    if (bytes == SOCKET_ERROR) {
        std::cerr << "接收文件名失败" << std::endl;
        return nullptr;
    } else {
        file_name[bytes] = '\0';
        std::cout << "接收到文件名：" << file_name << std::endl;
        return file_name;
    }
}

bool server_establish_connection(SOCKET sockfd, sockaddr_in &client_addr,
                                 int addr_len) {
    std::cout << "等待客户端连接请求..." << std::endl;
    Packet packet, ack_packet;

    // 接收客户端的 SYN 请求
    recvfrom(sockfd, (char *)&packet, sizeof(packet), 0,
             (struct sockaddr *)&client_addr, &addr_len);
    if (strcmp(packet.data, "SYN") == 0) { // 判断是否是 SYN 请求
        std::cout << "收到 SYN 请求" << std::endl;

        // 发送 SYN-ACK 响应
        ack_packet.seq_num = 0;
        strcpy(ack_packet.data, "SYN-ACK");
        ack_packet.checksum = calculate_checksum(ack_packet);
        sendto(sockfd, (char *)&ack_packet, sizeof(ack_packet), 0,
               (struct sockaddr *)&client_addr, addr_len);
        std::cout << "发送 SYN-ACK 响应" << std::endl;

        // 等待客户端的 ACK 确认消息
        recvfrom(sockfd, (char *)&packet, sizeof(packet), 0,
                 (struct sockaddr *)&client_addr, &addr_len);
        if (strcmp(packet.data, "ACK") == 0) { // 判断是否是 ACK 确认消息
            std::cout << "收到 ACK 确认消息，连接建立" << std::endl;
            return true;
        }
    }
    return false;
}

// 服务器端断开连接：在收到客户端的 FIN 请求后，发送 ACK 确认消息
int server_close_connection(SOCKET sockfd, sockaddr_in &client_addr,
                             int addr_len) {
    Packet packet;
    packet.seq_num = 1;
    strcpy(packet.data, "ACK");
    packet.checksum = calculate_checksum(packet);
    sendto(sockfd, (char *)&packet, sizeof(packet), 0,
           (struct sockaddr *)&client_addr, addr_len);
    std::cout << "发送 ACK 确认消息" << std::endl;

    return 0;
}