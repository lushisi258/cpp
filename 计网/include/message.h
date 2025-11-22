#include <arpa/inet.h>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <iostream>

// 消息类型枚举
enum class MessageType : uint32_t { JOIN = 1, LEAVE = 2, TEXT = 3, ERROR = 4 };

struct Message {
    // 消息类型；消息时间戳；用户名；消息内容
    MessageType type;
    uint64_t timestamp;
    std::string username;
    std::string content;

    // 构造函数
    Message();
    Message(MessageType t, const std::string &u, const std::string &c);
    Message(MessageType t, uint64_t ts, const std::string &u, const std::string &c);

    // 消息序列化
    std::string serialize() const;
};

// 消息反序列化
Message deserialize(const std::string &s);