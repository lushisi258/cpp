#pragma once
#include <vector>
#include <thread>
#include <mutex>
#include <netinet/in.h>

class ChatServer {
public:
    ChatServer(int port);
    ~ChatServer();
    void start();

private:
    int server_fd;
    int port;
    std::vector<int> clients;
    std::mutex clients_mutex;
    bool running;

    void handle_client(int client_sock);
    void broadcast(const std::string &msg, int exclude_sock = -1);
};
