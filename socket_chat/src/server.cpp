#include "../include/msg.h"
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>

std::vector<SOCKET> clients;   // 存储所有客户端socket
std::mutex mtx;                // 用于保护客户端列表的互斥锁
std::vector<Message> messages; // 存储所有收到的消息

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
            std::cerr << "Client disconnected or error occurred" << std::endl;
            break;
        }
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
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    sockaddr_in serverAddr, clientAddr;
    int clientAddrLen = sizeof(clientAddr);

    // 初始化 Winsock
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // 创建服务器套接字
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        return 1;
    }

    // 绑定服务器地址和端口
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; // 接收所有地址的连接
    serverAddr.sin_port = htons(8888);
    if (bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) ==
        SOCKET_ERROR) {
        std::cerr << "Binding failed" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // 监听端口
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server listening on port 8888..." << std::endl;

    // 主循环：接收客户端连接并为每个客户端创建线程
    while (true) {
        clientSocket =
            accept(serverSocket, (sockaddr *)&clientAddr, &clientAddrLen);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed" << std::endl;
            continue;
        }

        std::cout << "New client connected!" << std::endl;

        {
            std::lock_guard<std::mutex> lock(mtx);
            clients.push_back(clientSocket); // 将新客户端添加到列表中
        }

        // 为每个客户端创建一个新线程来处理通信
        std::thread client_thread(handle_client, clientSocket);
        client_thread.detach(); // 分离线程，主线程不需要等待
    }

    // 关闭服务器套接字
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
