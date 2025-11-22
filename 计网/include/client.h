#ifndef CLIENT_H
#define CLIENT_H

#include "message.h"
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

// 线程安全的消息队列
template <typename T> class ThreadSafeQueue {
  private:
    std::queue<T> queue;
    std::mutex mutex;
    std::condition_variable condition;

  public:
    void push(const T &value) {
        std::lock_guard<std::mutex> lock(mutex);
        queue.push(value);
        condition.notify_one();
    }

    bool pop(T &value) {
        std::unique_lock<std::mutex> lock(mutex);
        if (queue.empty()) {
            return false;
        }
        value = queue.front();
        queue.pop();
        return true;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.empty();
    }
};

class ChatClient {
  private:
    // 连接状态
    int sock_fd;
    std::atomic<bool> connected;
    std::atomic<bool> running;

    // 用户信息
    std::string username;
    std::string server_host;
    int server_port;

    // 线程
    std::thread receive_thread;
    std::thread input_thread;

    // 消息队列
    ThreadSafeQueue<std::string> message_queue;
    ThreadSafeQueue<Message> display_queue;

  public:
    ChatClient();
    ~ChatClient();

    // 连接管理
    bool connectToServer(const std::string &host, int port,
                         const std::string &user);
    void disconnect();
    bool isConnected() const { return connected; }

    // 消息处理
    void sendMessage(const std::string &content);
    void start();
    void stop();

  private:
    // 内部函数
    void receiveMessages();
    void processUserInput();
    void displayMessages();
    void processServerMessage(const Message &message);
    void clearScreen();

    // 工具函数
    std::string getCurrentTime();
    void printMessage(const std::string &type, const std::string &sender,
                      const std::string &content, const std::string &time = "");
};

#endif // CLIENT_H