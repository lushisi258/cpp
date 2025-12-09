#include "server.h"
#include "message.h"
#include <algorithm>
#include <arpa/inet.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

ChatServer::ChatServer(int port) : port(port), running(true) {}

ChatServer::~ChatServer() {
    running = false;
    close(server_fd);
}

void ChatServer::start() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    bind(server_fd, (sockaddr *)&addr, sizeof(addr));
    listen(server_fd, 5);

    std::cout << "Server started on port " << port << std::endl;

    while (running) {
        int client_sock = accept(server_fd, nullptr, nullptr);
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.push_back(client_sock);
        std::thread(&ChatServer::handle_client, this, client_sock).detach();
    }
}

void ChatServer::handle_client(int client_sock) {
    char buffer[4096];
    while (true) {
        int len = recv(client_sock, buffer, sizeof(buffer), 0);
        if (len <= 0)
            break;

        try {
            Message msg = deserialize(std::string(buffer, len));
            std::cout << '[' << msg.username << ']' << msg.content << std::endl;
            broadcast(msg.serialize(), client_sock);
        } catch (...) {
            Message err(MessageType::ERROR, "server", "deserialize error");
            send(client_sock, err.serialize().data(), err.serialize().size(),
                 0);
        }
    }

    close(client_sock);
    std::lock_guard<std::mutex> lock(clients_mutex);
    clients.erase(std::remove(clients.begin(), clients.end(), client_sock),
                  clients.end());
}

void ChatServer::broadcast(const std::string &msg, int exclude_sock) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (int sock : clients) {
        if (sock != exclude_sock) {
            send(sock, msg.data(), msg.size(), 0);
        }
    }
}

int main(int argc, char *argv[]) {
    int port = 12345;
    if (argc > 1)
        port = std::stoi(argv[1]);
    ChatServer server(port);
    server.start();
    return 0;
}