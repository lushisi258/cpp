#include "file_receiver.h"

int rdt_recv_file(SOCKET sockfd, sockaddr_in &client_addr, int addr_len) {
    server_establish_connection(sockfd, client_addr, sizeof(client_addr));

    char *filename = get_file_name(client_addr, sockfd);
    receive_file(sockfd, client_addr, sizeof(client_addr), filename);

    server_close_connection(sockfd, client_addr, sizeof(client_addr));

    return 0;
}

int main() {
    WSADATA wsaData;
    if (!initialize_winsock(wsaData))
        return -1;

    SOCKET sockfd = create_socket();
    if (sockfd == INVALID_SOCKET)
        return -1;

    sockaddr_in server_addr, client_addr;
    setup_server_address(server_addr, 2234);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
        SOCKET_ERROR) {
        std::cerr << "绑定失败：" << WSAGetLastError() << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return -1;
    } else {
        std::cout << "服务器监听在2234端口" << std::endl;
    }

    while(true) {   
        rdt_recv_file(sockfd, client_addr, sizeof(client_addr));
    }

    return 0;
}
