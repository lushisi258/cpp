#include "../include/receiver.h"

Receiver::Receiver() {

    // 初始化接收方套接字，监听MYPROTO类型的数据报
    recv_fd = socket(AF_INET, SOCK_RAW, MYPROTO);

    if (recv_fd < 0)
        throw std::runtime_error("接收方套接字创建失败");

    status = Status::LISTEN;

    // 持续接收数据报
    worker = std::thread([this]() {
        uint8_t buf[2048];
        while (true) {
            ssize_t n =
                recvfrom(recv_fd, buf, sizeof(buf), 0, nullptr, nullptr);
            if (n > 0)
                handle(buf, n);
        }
    });
    worker.detach();

    std::string cmd;
    while (cmd != "/quit") {
        std::cin >> cmd;
        cmd_handle(cmd);
    }
}

int Receiver::handle(uint8_t *buf, ssize_t n) {
    // 计算 IP 头长度
    int ihl = (buf[0] & 0x0F) * 4;

    // 指向 MYHDR
    MYHDR *hdr = reinterpret_cast<MYHDR *>(buf + ihl);

    uint32_t sender_seq = ntohl(hdr->seq);
    uint32_t sender_ack = ntohl(hdr->ack);
    uint16_t flags = ntohs(hdr->flags);

    switch (status) {
    case Status::LISTEN: {
        // 处理第一次握手
        recv_seq = std::rand();
        uint32_t ack1 = sender_ack + 1;
        std::vector<uint8_t> seg =
            make_segment(recv_seq, ack1, TCP_FLAG_SYN | TCP_FLAG_ACK, 0);
        status = Status::SYN_RCVD;
        break;
    }
    case Status::SYN_RCVD: {
        // 处理第三次握手
        if (seq == r)
            break;
    }
    case Status::ESTABLISHED: {
        break;
    }
    default: {
        break;
    }
    }
};

int main() {
    Receiver recv = Receiver();
    return 0;
}