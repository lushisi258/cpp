#ifndef MSG_H
#define MSG_H

#include <string>
#include <vector>

// 定义解析后的消息结构体
struct Message {
    std::string username;
    std::string message;
    std::string timestamp;
};

// 解析消息格式 "username$$message$$timestamp"
Message parse_msg(const std::string &input);

// 格式化输出msg
void print_format_msg(const Message &msg);

#endif // MSG_H