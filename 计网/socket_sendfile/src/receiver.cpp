#include "./include/receiver.h"

Receiver::Receiver(std::string host = "127.0.0.1", int port = 3456) {
    // 清空地址缓存，赋值地址结构体
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(3456);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 初始化服务器监听套接字
    listen_fd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);

    // // 绑定套接字到服务器地址
    // int bind_status = bind(listen_fd, (const struct sockaddr *)&server_addr,
    //                        sizeof(server_addr));

    // 开始监听连接
    int listen_status = listen(listen_fd, 10);
    while ((conn_fd = accept(listen_fd, nullptr, nullptr)) != -1) {
        handle(conn_fd);
    }
}

int Receiver::handle(int socket_fd) {
    while (recv)
        ;
    return 0;
}