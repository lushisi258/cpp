#include "file_receiver.h"

void receive_file(SOCKET sockfd, sockaddr_in &client_addr, int addr_len,
                  const char *output_filename) {
    std::ofstream file(output_filename, std::ios::binary);
    if (!file) {
        std::cerr << "创建文件失败：" << output_filename << std::endl;
        return;
    }

    Packet packet, ack_packet;
    int expected_seq_num = 0;

    while (true) {
        int bytes = recvfrom(sockfd, (char *)&packet, sizeof(packet), 0,
                             (struct sockaddr *)&client_addr, &addr_len);
        if (bytes <= 0)
            continue;
        if (strcmp(packet.data, "FIN") == 0) {
            std::cout << "收到 FIN 请求" << std::endl;
            break;
        }

        int checksum = calculate_checksum(packet);
        if (checksum == packet.checksum && packet.seq_num == expected_seq_num) {

            file.write(packet.data, bytes - offsetof(Packet, data));
            std::cout << "接收到第" << packet.seq_num << "个包，其校验和为" << packet.checksum << std::endl;

            ack_packet.seq_num = expected_seq_num;
            sendto(sockfd, (char *)&ack_packet, sizeof(ack_packet), 0,
                   (struct sockaddr *)&client_addr, addr_len);

            expected_seq_num++;
        } else {
            std::cerr << "包错误" << std::endl;
        }
    }
    std::cout << "文件接收完毕" << std::endl;
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