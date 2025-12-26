#include "../include/sender.h"

Sender::Sender(uint16_t port) {
    sockfd = socket(AF_INET, SOCK_RAW, MYPROTO);
    if (sockfd < 0) {
        perror("socket");
        throw std::runtime_error("failed to create raw socket");
    }

    local_port = port;
    status = Status::CLOSED;

    memset(&peer_addr, 0, sizeof(peer_addr));
    peer_addr.sin_family = AF_INET;
}

int Sender::connect(const std::string &peer_ip, uint16_t peer_port) {
    uint8_t buf[2048];

    peer_addr.sin_addr.s_addr = inet_addr(peer_ip.c_str());
    peer_addr.sin_port = htons(peer_port);

    /* ========== 第一次握手：发送 SYN ========== */

    iss = std::rand();
    snd_nxt = iss;
    rcv_nxt = 0;

    uint16_t flags = TCP_FLAG_SYN;

    std::vector<uint8_t> syn_seg = make_segment(snd_nxt, 0, flags, 0);

    send_seg(syn_seg.data(), syn_seg.size());

    snd_nxt += 1;
    status = Status::SYN_SENT;

    /* ========== 等待第二次握手：SYN+ACK ========== */
    std::cout << "等待第二次握手" << std::endl;

    int n = recvfrom(sockfd, buf, sizeof(buf), 0, nullptr, nullptr);
    if (n <= 0)
        return -1;

    int ihl = (buf[0] & 0x0F) * 4;
    MYHDR *hdr = reinterpret_cast<MYHDR *>(buf + ihl);

    uint32_t peer_seq = ntohl(hdr->seq);
    uint32_t peer_ack = ntohl(hdr->ack);
    uint16_t peer_flags = ntohs(hdr->flags);

    std::cout << "收到第二次握手消息" << std::endl;

    if (status == Status::SYN_SENT) {
        if ((peer_flags & (TCP_FLAG_SYN | TCP_FLAG_ACK)) ==
                (TCP_FLAG_SYN | TCP_FLAG_ACK) &&
            peer_ack == snd_nxt) {

            /* ========== 第三次握手：ACK ========== */
            std::cout << "等待第三次握手" << std::endl;

            rcv_nxt = peer_seq + 1;

            std::vector<uint8_t> ack_seg =
                make_segment(snd_nxt, rcv_nxt, TCP_FLAG_ACK, 0);

            send_seg(ack_seg.data(), ack_seg.size());

            status = Status::ESTABLISHED;

            std::cout << "握手成功" << std::endl;

            return 0;
        }
    }

    return -1;
}

int Sender::send_test() {
    std::string msg = "hello world\n";
    const uint8_t *payload = reinterpret_cast<const uint8_t *>(msg.data());

    std::vector<uint8_t> seg =
        make_segment(snd_nxt, 0, TCP_FLAG_ACK, 0, payload, msg.size());

    sendto(sockfd, seg.data(), seg.size(), 0, (sockaddr *)&peer_addr,
           sizeof(peer_addr));
}

std::string status_to_string(Status s) {
    switch (s) {
    case Status::CLOSED:
        return "CLOSED";
    case Status::LISTEN:
        return "LISTEN";
    case Status::SYN_SENT:
        return "SYN_SENT";
    case Status::SYN_RCVD:
        return "SYN_RCVD";
    case Status::ESTABLISHED:
        return "ESTABLISHED";
    case Status::FIN_WAIT:
        return "FIN_WAIT";
    case Status::LAST_ACK:
        return "LAST_ACK";
    default:
        return "UNKNOWN";
    }
}

int Sender::show_status() {
    std::cout << "Current Status: " << status_to_string(this->status)
              << std::endl;
    return 0;
}

int Sender::disconnect() {
    // 以后实现 FIN
    return 0;
}

int main() {
    Sender s(2349);
    s.connect("127.0.0.1", 2359);
    s.show_status();
    s.send_test();
    s.show_status();

    std::string x;
    std::cin >> x;

    return 0;
}