#include "client.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include "message.h"

ChatClient::ChatClient(const std::string &host, int port) {
    sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &addr.sin_addr);

    connect(sock, (sockaddr*)&addr, sizeof(addr));
}

void ChatClient::start(const std::string &u) {
    username = u;

    Message join(MessageType::JOIN, username, username + " joined");
    auto j = join.serialize();
    send(sock, j.data(), j.size(), 0);

    std::thread(&ChatClient::sender, this).detach();
    std::thread(&ChatClient::receiver, this).join();
}

void ChatClient::sender() {
    std::string text;
    while (std::getline(std::cin, text)) {
        Message msg(MessageType::TEXT, username, text);
        auto s = msg.serialize();
        send(sock, s.data(), s.size(), 0);
    }
}

void ChatClient::receiver() {
    char buffer[4096];
    while (true) {
        int len = recv(sock, buffer, sizeof(buffer), 0);
        if (len <= 0) break;

        try {
            Message msg = deserialize(std::string(buffer, len));
            std::cout << "[" << msg.username << "] " << msg.content << std::endl;
        } catch (...) {}
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "用法: " << argv[0] << " <服务器IP> <用户名> [端口]" << std::endl;
        return 1;
    }
    std::string host = argv[1];
    std::string username = argv[2];
    int port = 12345;
    if (argc > 3) port = std::stoi(argv[3]);
    ChatClient client(host, port);
    client.start(username);
    return 0;
}