#include "server.h"

ChatServer::ChatServer() : server_fd(-1), is_running(false) {}

ChatServer::~ChatServer() { stop(); }

bool ChatServer::start(int port) {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return false;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
        0) {
        std::cerr << "Bind failed." << std::endl;
        return false;
    }

    if (listen(server_fd, 5) < 0) {
        std::cerr << "Listen failed." << std::endl;
        return false;
    }

    is_running = true;
    accept_thread = std::thread(&ChatServer::acceptConnections, this);
    return true;
}

void ChatServer::stop() {
    if (is_running) {
        is_running = false;
        close(server_fd);
        if (accept_thread.joinable()) {
            accept_thread.join();
        }
        cleanup();
    }
}

void ChatServer::addClient(int client_fd, const std::string &username) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    clients[client_fd] = {client_fd, username, std::thread(), true};
}

void ChatServer::removeClient(int client_fd) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    if (clients.find(client_fd) != clients.end()) {
        clients[client_fd].connected = false;
        if (clients[client_fd].handler_thread.joinable()) {
            clients[client_fd].handler_thread.join();
        }
        close(client_fd);
        clients.erase(client_fd);
    }
}

void ChatServer::showOnlineUsers() {
    auto users = getOnlineUsers();
    std::cout << "当前在线用户：";
    for (std::string user : users) {
        std::cout << ' ' << user;
    }
    std::cout << std::endl;
}

void ChatServer::broadcastMessage(const Message &message, int exclude_fd) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (const auto &[fd, client] : clients) {
        if (fd != exclude_fd && client.connected) {
            send(fd, message.serialize().c_str(), message.serialize().size(),
                 0);
        }
    }
}

void ChatServer::handleClient(int client_fd) {
    try {
        char buffer[4096];
        while (is_running) {
            // 接收数据
            ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
            if (bytes_received == 0) {
                // 客户端断开连接
                std::cout << "客户端 " << client_fd << " 已断开连接。"
                          << std::endl;
                break;
            } else if (bytes_received < 0) {
                // 接收失败
                std::cerr << "接收客户端 " << client_fd << " 数据时出错。"
                          << std::endl;
                break;
            }

            // 解析消息
            std::string data(buffer, bytes_received);
            try {
                Message message = deserialize(data);
                // 处理消息
                processMessage(client_fd, message);
            } catch (const std::exception &e) {
                std::cerr << "解析客户端 " << client_fd << " 消息时出错："
                          << e.what() << std::endl;
                continue;
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "处理客户端 " << client_fd << " 时发生异常：" << e.what()
                  << std::endl;
    }

    // 清理客户端资源
    removeClient(client_fd);
}

std::vector<std::string> ChatServer::getOnlineUsers() const {
    std::vector<std::string> users;
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (const auto &[fd, client] : clients) {
        if (client.connected) {
            users.push_back(client.username);
        }
    }
    return users;
}

size_t ChatServer::getClientCount() const {
    std::lock_guard<std::mutex> lock(clients_mutex);
    return clients.size();
}

void ChatServer::acceptConnections() {
    while (is_running) {
        socklen_t addr_len = sizeof(server_addr);
        int client_fd =
            accept(server_fd, (struct sockaddr *)&server_addr, &addr_len);
        if (client_fd < 0) {
            if (is_running) {
                std::cerr << "Accept failed." << std::endl;
            }
            continue;
        }
        // Placeholder for username retrieval
        std::string username = "User" + std::to_string(client_fd);
        addClient(client_fd, username);
        clients[client_fd].handler_thread =
            std::thread(&ChatServer::handleClient, this, client_fd);
    }
}

void ChatServer::processMessage(int client_fd, const Message &message) {
    switch (message.type) {
    case MessageType::TEXT:
        broadcastMessage(message, client_fd);
        break;
    default:
        sendErrorMessage(client_fd, "Unknown message type.");
        break;
    }
}

void ChatServer::sendErrorMessage(int client_fd, const std::string &error_msg) {
    std::cout << error_msg << std::endl;
    Message errorMessage(MessageType::ERROR, "System", error_msg);
    send(client_fd, errorMessage.serialize().c_str(),
         errorMessage.serialize().size(), 0);
}

void ChatServer::cleanup() {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (auto &[fd, client] : clients) {
        client.connected = false;
        if (client.handler_thread.joinable()) {
            client.handler_thread.join();
        }
        close(fd);
    }
    clients.clear();
}

int main() {
    ChatServer server;
    server.start(5566);
    std::cout << "服务器已启动，按回车键停止服务器" << std::endl;
    std::cin.get();
    server.stop();
    return 0;
}