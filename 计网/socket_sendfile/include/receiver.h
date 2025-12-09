#include "./include/sender.h"

class Receiver {
    // 服务器监听套接字
    int listen_fd;
    // 接收到的连接的套接字
    int conn_fd;
    // 服务器地址
    struct sockaddr_in server_addr;

    Receiver(std::string host = "127.0.0.1", int port = 3456);
    int handle(int socket_fd);
    ~Receiver();
};