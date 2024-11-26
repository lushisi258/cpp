#include "../include/msg.h"
#include <algorithm>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>

WSADATA wsaData;                      // Winsock 数据结构
SOCKET server_socket, client_socket;  // 服务器套接字和客户端套接字
sockaddr_in server_addr, client_addr; // 服务器地址和客户端地址
std::vector<SOCKET> clients;          // 存储所有客户端socket
std::vector<Message> messages;        // 存储所有收到的消息
std::mutex mtx;                       // 用于保护客户端列表的互斥锁

// 广播消息给其他客户端
void broadcast_message(const std::string &message, SOCKET sender_socket) {
    std::lock_guard<std::mutex> lock(mtx); // 确保线程安全访问

    for (SOCKET client_socket : clients) {
        // 不发送给发送者自己
        if (client_socket != sender_socket) {
            send(client_socket, message.c_str(), message.size(), 0);
        }
    }
}

// 处理客户端的连接
void handle_client(SOCKET client_socket) {
    char buffer[1024];

    while (true) {
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received == SOCKET_ERROR || bytes_received == 0) {
            // 断开连接时移除客户端
            {
                std::lock_guard<std::mutex> lock(mtx);
                auto it =
                    std::remove(clients.begin(), clients.end(), client_socket);
                clients.erase(it, clients.end());
                print_color_msg("Removed disconnected client\n", 2);
            }
            break;
        }
        // 末尾添加空字符，以便将其转换为字符串
        buffer[bytes_received] = '\0';

        // 转换格式并保存消息
        std::string message(buffer);
        Message msg = parse_msg(message);
        {
            std::lock_guard<std::mutex> lock(mtx);
            messages.push_back(msg);
        }
        std::cout << "Recv msg from ";
        print_format_msg(msg);

        // 广播消息给其他所有客户端
        broadcast_message(message, client_socket);
    }
    closesocket(client_socket);
}

int main() {
    int client_addr_len = sizeof(client_addr); // 客户端地址长度

    // 初始化 Winsock
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // 创建服务器套接字
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        return 1;
    }

    // 绑定服务器地址和端口
    server_addr.sin_family = AF_INET;         // 设置地址族为 IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; // 接收所有地址的连接
    server_addr.sin_port = htons(8888);       // 设置服务器端口
    if (bind(server_socket, (sockaddr *)&server_addr, sizeof(server_addr)) ==
        SOCKET_ERROR) {
        std::cerr << "Binding failed" << std::endl;
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // 监听端口
    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed" << std::endl;
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    print_color_msg("Server started successfully on port 8888\n", 1);

    // 接收客户端连接并为每个客户端创建线程
    while (true) {
        client_socket =
            accept(server_socket, (sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == INVALID_SOCKET) {
            std::cerr << "Accept failed" << std::endl;
            continue;
        }

        print_color_msg("New client connected\n", 1);

        {
            std::lock_guard<std::mutex> lock(mtx);
            clients.push_back(client_socket); // 将新客户端添加到列表中
        }

        // 为每个客户端创建一个新线程来处理通信
        std::thread client_thread(handle_client, client_socket);
        client_thread.detach(); // 分离线程，主线程不需要等待
    }

    // 关闭服务器套接字
    closesocket(server_socket);
    WSACleanup();

    return 0;
}
