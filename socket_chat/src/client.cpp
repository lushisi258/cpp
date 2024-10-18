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

WSADATA wsaData;
SOCKET clientSocket;
sockaddr_in serverAddr;
std::condition_variable cv;    // 条件变量
std::mutex mtx_done;           // 用于保护 done 状态
std::mutex mtx;                // 用于保护消息列表
std::vector<Message> messages; // 保存所有消息的表
std::string username;
bool done = false;
long long unsigned int msg_index =
    0; // 消息索引，用于跟踪打印位置(和vector.size()保持类型一致)

// 获取当前时间
std::string get_current_time() {
    std::time_t now = std::time(nullptr); // 获取当前时间戳
    std::tm *ltm = std::localtime(&now);  // 将时间戳转换为本地时间

    std::ostringstream oss;
    oss << std::put_time(ltm, "%Y-%m-%d %H:%M:%S"); // 格式化时间
    return oss.str(); // 返回格式化后的时间字符串
}

void recv_msg() {
    while (true) {
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
        Message msg = parse_msg(buffer);
        {
            std::lock_guard<std::mutex> lock(mtx);
            messages.push_back(msg);
        }
        print_format_msg(msg);
    }
}

void send_msg() {
    while (true) {
        std::string input;
        std::getline(std::cin, input);
        if (input == "/exit" || input == "/quit" || input == "/q") {
            {
                std::lock_guard<std::mutex> lock(mtx_done);
                done = true;
            }
            cv.notify_all();
            break;
        } else if (!input.empty()) {
            Message msg;
            msg.username = username;
            msg.message = input;
            msg.timestamp = get_current_time();
            {
                std::lock_guard<std::mutex> lock(mtx);
                messages.push_back(msg);
            }

            std::string fullMessage =
                username + '$' + input + '$' + msg.timestamp;
            int totalSent = 0;
            int toSend = fullMessage.length();
            while (totalSent < toSend) {
                int sent = send(clientSocket, fullMessage.c_str() + totalSent,
                                toSend - totalSent, 0);
                if (sent == SOCKET_ERROR) {
                    std::cerr << "Send failed" << std::endl;
                    {
                        std::lock_guard<std::mutex> lock(mtx_done);
                        done = true;
                    }
                    cv.notify_all();
                    break;
                }
                totalSent += sent;
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
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(8888);
    if (connect(clientSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) ==
        SOCKET_ERROR) {
        std::cerr << "Connection failed" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to server" << std::endl;

    // 启动消息处理线程
    std::thread recv_thread(recv_msg);
    std::thread input_thread(send_msg);
    {
        std::unique_lock<std::mutex> lock(mtx_done);
        cv.wait(lock, [] { return done; }); // 当done为true时，解除阻塞
    }

    // 关闭套接字
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
