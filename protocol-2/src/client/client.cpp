#include "file_sender.h"

int rdt_send_file(SOCKET sockfd, sockaddr_in &server_addr, int addr_len) {
    if (client_establish_connection(sockfd, server_addr, sizeof(server_addr))) {
        char filename[256];
        std::cout << "输入要发送的文件名：";
        std::cin >> filename;

        send_file_name(sockfd, server_addr, sizeof(server_addr), filename);
        
        auto time1 = std::chrono::steady_clock::now();
        send_file(sockfd, server_addr, sizeof(server_addr), filename);
        auto time2 = std::chrono::steady_clock::now();
        auto time_cost =
            std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time1)
                .count();
        std::cout << "传输时间：" << time_cost << "ms" << std::endl;

        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        auto file_size = file.tellg();
        // 计算吞吐率
        std::cout << "吞吐率：" << file_size / time_cost << "KB/s" << std::endl;

        close_connection(sockfd, server_addr, sizeof(server_addr));
    } else {
        std::cerr << "连接失败" << std::endl;
    }
    return 0;
}

int main() {
#ifdef _WIN32
    // 设置控制台输出为 UTF-8 编码
    SetConsoleOutputCP(CP_UTF8);
#endif

    WSADATA wsaData;
    if (!initialize_winsock(wsaData))
        return -1;

    SOCKET sockfd = create_socket();
    if (sockfd == INVALID_SOCKET)
        return -1;

    sockaddr_in server_addr;
    setup_server_address(server_addr, 2233);

    rdt_send_file(sockfd, server_addr, sizeof(server_addr));

    closesocket(sockfd);

    WSACleanup();
    return 0;
}
