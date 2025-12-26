#include "../include/receiver.h"

Receiver::Receiver() {
    sockfd = socket(AF_INET, SOCK_RAW, MYPROTO);
    if (sockfd < 0)
        throw std::runtime_error("failed to create raw socket");

    status = Status::LISTEN;

    recv_thread = std::thread([this]() {
        uint8_t buf[2048];
        while (true) {
            ssize_t n = recvfrom(sockfd, buf, sizeof(buf), 0, nullptr, nullptr);
            std::cout << "DEBUG: 收到原始字节数: " << n << std::endl;
            if (n > 0) {
                std::cout << "收到数据报" << std::endl;
                handle_packet(buf, n);
            }
        }
    });
    recv_thread.detach();
}

int Receiver::handle_packet(uint8_t *buf, ssize_t n) {
    // 跳过 IP 头
    int ihl = (buf[0] & 0x0F) * 4;
    MYHDR *hdr = reinterpret_cast<MYHDR *>(buf + ihl);

    uint32_t peer_seq = ntohl(hdr->seq);
    uint32_t peer_ack = ntohl(hdr->ack);
    uint16_t flags = ntohs(hdr->flags);

    switch (status) {

    case Status::LISTEN: {
        // 只接受 SYN
        if (flags & TCP_FLAG_SYN) {
            irs = peer_seq;
            rcv_nxt = irs + 1;

            snd_nxt = std::rand();

            std::vector<uint8_t> syn_ack =
                make_segment(snd_nxt, rcv_nxt, TCP_FLAG_SYN | TCP_FLAG_ACK, 0);

            send_seg(syn_ack.data(), syn_ack.size());

            snd_nxt += 1;
            status = Status::SYN_RCVD;
        }
        break;
    }

    case Status::SYN_RCVD: {
        // 等第三次握手 ACK
        if ((flags & TCP_FLAG_ACK) && peer_ack == snd_nxt) {

            status = Status::ESTABLISHED;
            std::cout << "Receiver: connection established\n";
        }
        break;
    }

    case Status::ESTABLISHED: {
        // 连接成功，处理数据
        uint8_t *payload = buf + ihl + sizeof(MYHDR);
        size_t payload_len = n - ihl - sizeof(MYHDR);

        if (payload_len > 0) {
            std::cout.write(reinterpret_cast<char *>(payload), payload_len);
            std::cout << std::endl;
        }

        break;
    }

    default:
        break;
    }

    return 0;
}

Receiver::~Receiver() {}

int main() {
    Receiver rcv;

    std::string x;
    std::cin >> x;
    return 0;
}