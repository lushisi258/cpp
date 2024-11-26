#include "../include/msg.h"
#include <condition_variable>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>

WSADATA wsaData;               // Winsock 数据结构
SOCKET clientSocket;           // 客户端套接字
sockaddr_in serverAddr;        // 服务器地址
std::condition_variable cv;    // 条件变量
std::mutex mtx_done;           // 用于保护 done 状态
std::mutex mtx_msgs;           // 用于保护消息列表
std::vector<Message> messages; // 保存所有消息的表
std::string username;          // 用户名
bool done = false;             // 客户端是否退出的标志位

// 获取当前时间
std::string get_current_time() {
    std::time_t now = std::time(nullptr); // 获取当前时间戳
    std::tm *ltm = std::localtime(&now);  // 将时间戳转换为本地时间

    std::ostringstream oss;
    oss << std::put_time(ltm, "%Y-%m-%d %H:%M:%S"); // 格式化时间
    return oss.str(); // 返回格式化后的时间字符串
}

void recv_msg() {
    while (!done) {
        char buffer[1024];
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived == SOCKET_ERROR || bytesReceived == 0) {
            std::cerr << "Connection closed" << std::endl;
            {
                std::lock_guard<std::mutex> lock(mtx_done);
                done = true;
            }
            cv.notify_all();
            break;
        }
        buffer[bytesReceived] = '\0';
        std::string message(buffer);
        Message msg = parse_msg(message);
        {
            std::lock_guard<std::mutex> lock(mtx_msgs);
            messages.push_back(msg);
        }
        print_format_msg(msg);
    }
}

void send_msg() {
    while (!done) {
        std::string input;
        std::getline(std::cin, input);
        if (input == "/exit" || input == "/quit" || input == "/q") {
            {
                std::lock_guard<std::mutex> lock(mtx_done);
                done = true;
            }
            // 客户端发出退出消息后，关闭套接字并通知其他线程
            closesocket(clientSocket);
            cv.notify_all();
            break;
        } else if (!input.empty()) {
            Message msg;
            msg.username = username;
            msg.message = input;
            msg.timestamp = get_current_time();
            {
                std::lock_guard<std::mutex> lock(mtx_msgs);
                messages.push_back(msg);
            }

            std::string string_msg =
                username + '$' + input + '$' + msg.timestamp;
            int total_sent = 0;                // 已发送的字节数
            int to_send = string_msg.length(); // 要发送的字节数
            while (total_sent < to_send) {
                // 发送消息
                int sent = send(clientSocket, string_msg.c_str() + total_sent,
                                to_send - total_sent, 0);
                if (sent == SOCKET_ERROR) {
                    std::cerr << "Send failed" << std::endl;
                    {
                        std::lock_guard<std::mutex> lock(mtx_done);
                        done = true;
                    }
                    cv.notify_all();
                    break;
                }
                total_sent += sent;
            }
        }
    }
}

int main() {
    // 初始化Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return 1;
    }

    std::cout << "Enter your username: ";
    std::getline(std::cin, username);

    // 创建套接字
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        return 1;
    }

    // 连接到服务器
    serverAddr.sin_family = AF_INET; // 设置地址族为 IPv4
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 设置服务器IP地址
    serverAddr.sin_port = htons(8888); // 设置服务器端口
    // 使用 connect 函数连接到服务器
    // 如果连接失败，关闭套接字并清理 Winsock
    if (connect(clientSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) ==
        SOCKET_ERROR) {
        std::cerr << "Connection failed" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    // 输出连接成功消息
    print_color_msg("Connected to server\n", 1);

    // 启动消息处理线程
    std::thread recv_thread(recv_msg);
    std::thread input_thread(send_msg);

    // 等待客户端退出
    recv_thread.join();
    input_thread.join();
    WSACleanup();
    return 0;
}
