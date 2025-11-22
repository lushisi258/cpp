#pragma once
#include <string>

class ChatClient {
public:
    ChatClient(const std::string &host, int port);
    void start(const std::string &username);

private:
    int sock;
    std::string username;

    void sender();
    void receiver();
};
