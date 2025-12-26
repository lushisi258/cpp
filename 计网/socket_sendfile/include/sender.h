#include "util.h"

class Sender {
  private:
    Status status = Status::CLOSED;

    int sockfd;            // raw socket fd
    uint16_t local_port;   // 本地端口（逻辑端口）
    sockaddr_in peer_addr; // 对端地址（IP）

    uint32_t iss;     // 初始发送序列号 initial send sequence
    uint32_t snd_nxt; // 下一个发送的序列号 next seq to send
    uint32_t rcv_nxt; // 期望的下一个序列号 next seq expected from peer

    std::thread recv_thread;

  public:
    explicit Sender(uint16_t port);

    int connect(const std::string &peer_ip, uint16_t peer_port);
    void send_file(const std::string &dir_path, const std::string &file_name);
    int disconnect();
};
