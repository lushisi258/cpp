#include "util.h"

class Receiver {
  private:
    Status status = Status::LISTEN;

    int sockfd;            // raw socket fd
    sockaddr_in peer_addr; // 对端（Sender）地址

    uint32_t irs;     // initial receive sequence
    uint32_t rcv_nxt; // next expected seq from peer
    uint32_t snd_nxt; // next seq to send (ACK / data)

    std::thread recv_thread;

  public:
    Receiver(uint16_t port);

    int handle_packet(uint8_t *buf, ssize_t len);
    ~Receiver();
};
