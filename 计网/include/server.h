#ifndef SERVER_H
#define SERVER_H

#include "message.h"
#include <atomic>
#include <cstring>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <vector>

// 客户端信息结构
struct ClientInfo {
    int fd;                     // 客户端socket文件描述符
    std::string username;       // 用户名
    std::thread handler_thread; // 处理该客户端的线程
    bool connected;             // 连接状态
};

class ChatServer {
  private:
    // 服务器状态
    int server_fd;                // 服务器socket
    std::atomic<bool> is_running; // 服务器运行状态
    std::thread accept_thread;    // 接受连接的线程

    // 客户端管理
    std::unordered_map<int, ClientInfo> clients; // 客户端列表
    mutable std::mutex clients_mutex;            // 保护clients的互斥锁

    // 网络地址
    struct sockaddr_in server_addr;

  public:
    ChatServer();
    ~ChatServer();

    // 服务器控制
    bool start(int port);
    void stop();
    bool isRunning() const { return is_running; }

    // 客户端管理
    void addClient(int client_fd,
                   const std::string &username); // 添加新连接的客户端
    void removeClient(int client_fd);            // 移除断开连接的客户端
    void showOnlineUsers();                      // 显示在线用户

    // 消息处理
    void broadcastMessage(const Message &message,
                          int exclude_fd = -1); // 向聊天室广播消息
    void handleClient(int client_fd);
    void sendErrorMessage(int client_fd, const std::string &error_msg);

    // 工具函数
    std::vector<std::string> getOnlineUsers() const;
    size_t getClientCount() const;

  private:
    // 内部函数
    void acceptConnections();
    void processMessage(int client_fd, const Message &message);
    void cleanup();
};

#endif // SERVER_H