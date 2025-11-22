#include "client.h"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <unistd.h>

ChatClient::ChatClient() : sock_fd(-1), connected(false), running(false) {}

ChatClient::~ChatClient() { disconnect(); }

bool ChatClient::connectToServer(const std::string &host, int port,
                                 const std::string &user) {
    server_host = host;
    server_port = port;
    username = user;

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        std::cerr << "Error: Unable to create socket." << std::endl;
        return false;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_host.c_str(), &server_addr.sin_addr) <= 0) {
        std::cerr << "Error: Invalid server address." << std::endl;
        return false;
    }

    if (connect(sock_fd, (struct sockaddr *)&server_addr,
                sizeof(server_addr)) == -1) {
        std::cerr << "Error: Unable to connect to server." << std::endl;
        return false;
    }

    connected = true;
    running = true;
    std::cout << "Connected to server." << std::endl;

    return true;
}

void ChatClient::disconnect() {
    if (connected) {
        close(sock_fd);
        connected = false;
        running = false;
        std::cout << "Disconnected from server." << std::endl;
    }
}

void ChatClient::sendMessage(const std::string &content) {
    if (!connected) {
        std::cerr << "Error: Not connected to server." << std::endl;
        return;
    }

    uint64_t timestamp = std::time(nullptr); // 获取当前时间戳
    Message message(MessageType::TEXT, timestamp, username, content);
    std::string serialized = message.serialize();
    if (send(sock_fd, serialized.c_str(), serialized.size(), 0) == -1) {
        std::cerr << "Error: Failed to send message." << std::endl;
    }
}

void ChatClient::start() {
    receive_thread = std::thread(&ChatClient::receiveMessages, this);
    input_thread = std::thread(&ChatClient::processUserInput, this);
}

void ChatClient::stop() {
    running = false;
    if (receive_thread.joinable())
        receive_thread.join();
    if (input_thread.joinable())
        input_thread.join();
}

void ChatClient::receiveMessages() {
    while (running) {
        char buffer[1024] = {0};
        int bytes_received = recv(sock_fd, buffer, sizeof(buffer), 0);
        if (bytes_received > 0) {
            Message message = deserialize(std::string(buffer, bytes_received));
            printMessage("Server", message.username, message.content,
                         std::to_string(message.timestamp));
        } else if (bytes_received == 0) {
            std::cerr << "Server disconnected." << std::endl;
            stop();
        } else {
            std::cerr << "Error receiving message." << std::endl;
        }
    }
}

void ChatClient::processUserInput() {
    std::string input;
    while (running) {
        std::getline(std::cin, input);
        if (input == "/quit") {
            std::cout << "Exiting chat..." << std::endl;
            stop();
        } else {
            sendMessage(input);
        }
    }
}

void ChatClient::displayMessages() {
    Message message(MessageType::TEXT, 0, "", ""); // 使用明确的构造函数初始化
    while (display_queue.pop(message)) {
        printMessage("Server", message.username, message.content,
                     std::to_string(message.timestamp));
    }
}

void ChatClient::processServerMessage(const Message &message) {
    display_queue.push(message);
}

void ChatClient::clearScreen() { std::cout << "\033[2J\033[1;1H"; }

std::string ChatClient::getCurrentTime() {
    // Placeholder for actual time retrieval logic
    return "00:00";
}

void ChatClient::printMessage(const std::string &type,
                              const std::string &sender,
                              const std::string &content,
                              const std::string &time) {
    std::cout << "[" << type << "] " << sender << " (" << time
              << "): " << content << std::endl;
}

int main() {
    ChatClient client;

    std::string server_host = "127.0.0.1";
    int server_port = 5566;
    std::string username;
    std::cout << "Enter your username: ";
    std::cin >> username;

    std::cin.ignore();

    if (!client.connectToServer(server_host, server_port, username)) {
        std::cerr << "Failed to connect to server. Exiting..." << std::endl;
        return 1;
    }

    client.start();

    std::cout << "Chat started. Type '/quit' to exit." << std::endl;

    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();

    client.stop();
    client.disconnect();

    return 0;
}