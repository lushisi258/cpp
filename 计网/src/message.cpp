#include <message.h>

// 64位主机序与网络序转换
static uint64_t htonll(uint64_t value) {
    static const int num = 42;
    if (*(const char *)&num == 42) { // 小端序
        uint32_t high = htonl((uint32_t)(value >> 32));
        uint32_t low = htonl((uint32_t)(value & 0xFFFFFFFFULL));
        return (((uint64_t)low) << 32) | high;
    } else {
        return value;
    }
}

static uint64_t ntohll(uint64_t value) {
    static const int num = 42;
    if (*(const char *)&num == 42) { // 小端序
        uint32_t high = ntohl((uint32_t)(value >> 32));
        uint32_t low = ntohl((uint32_t)(value & 0xFFFFFFFFULL));
        return (((uint64_t)low) << 32) | high;
    } else {
        return value;
    }
}

// 构造函数
Message::Message() {};
Message::Message(MessageType t, const std::string &u, const std::string &c)
    : type(t), username(u), content(c) {
    timestamp = static_cast<uint64_t>(std::time(nullptr));
}
Message::Message(MessageType t, const uint64_t ts, const std::string &u,
                 const std::string &c)
    : type(t), timestamp(ts), username(u), content(c) {}

// 序列化格式（网络字节序）：
// [类型uint32_t][时间戳uint64_t]
// [用户名长度uint32_t][用户名数据]
// [内容长度uint32_t][内容数据]
std::string Message::serialize() const {
    std::string out;
    out.reserve(4 + 8 + 4 + username.size() + 4 + content.size());

    // 消息类型
    uint32_t t = static_cast<uint32_t>(type);
    uint32_t t_net = htonl(t);
    out.append(reinterpret_cast<const char *>(&t_net), sizeof(t_net));

    // 时间戳
    uint64_t ts_net = htonll(timestamp);
    out.append(reinterpret_cast<const char *>(&ts_net), sizeof(ts_net));

    // 用户名（长度+数据）
    uint32_t uname_len = static_cast<uint32_t>(username.size());
    uint32_t uname_len_net = htonl(uname_len);
    out.append(reinterpret_cast<const char *>(&uname_len_net),
               sizeof(uname_len_net));
    if (uname_len)
        out.append(username.data(), uname_len);

    // 消息内容（长度+数据）
    uint32_t content_len = static_cast<uint32_t>(content.size());
    uint32_t content_len_net = htonl(content_len);
    out.append(reinterpret_cast<const char *>(&content_len_net),
               sizeof(content_len_net));
    if (content_len)
        out.append(content.data(), content_len);

    return out;
}

// 反序列化
Message deserialize(const std::string &s) {
    size_t offset = 0;

    // 检查最小长度
    if (s.size() < 4 + 8 + 4 + 4) {
        throw std::runtime_error("数据长度不足");
    }

    // 读取消息类型
    uint32_t t_net;
    memcpy(&t_net, s.data() + offset, sizeof(t_net));
    offset += sizeof(t_net);
    MessageType type = static_cast<MessageType>(ntohl(t_net));

    // 读取时间戳
    uint64_t ts_net;
    memcpy(&ts_net, s.data() + offset, sizeof(ts_net));
    offset += sizeof(ts_net);
    uint64_t timestamp = ntohll(ts_net);

    // 读取用户名
    uint32_t uname_len_net;
    memcpy(&uname_len_net, s.data() + offset, sizeof(uname_len_net));
    offset += sizeof(uname_len_net);
    uint32_t uname_len = ntohl(uname_len_net);

    if (offset + uname_len > s.size()) {
        throw std::runtime_error("用户名数据越界");
    }
    std::string username(s.data() + offset, uname_len);
    offset += uname_len;

    // 读取消息内容
    uint32_t content_len_net;
    memcpy(&content_len_net, s.data() + offset, sizeof(content_len_net));
    offset += sizeof(content_len_net);
    uint32_t content_len = ntohl(content_len_net);

    if (offset + content_len > s.size()) {
        throw std::runtime_error("内容数据越界");
    }
    std::string content(s.data() + offset, content_len);

    // 构造Message对象（需要添加带时间戳的构造函数）
    return Message(type, timestamp, username, content);
}