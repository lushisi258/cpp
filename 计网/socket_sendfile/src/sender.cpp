#include "../include/sender.h"

Sender::Sender(int s_port) {
    sender_fd = socket(AF_INET, SOCK_RAW, MYPROTO); // 初始化发送方套接字
    if (sender_fd < 0) {
        perror("socket");
        std::cout << "创建sender_fd失败" << std::endl;
    }
    sender_port = s_port;
    // 初始化接收方地址
    dst_addr.sin_family = AF_INET;
    dst_addr.sin_port = 0;
    dst_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
}

int Sender::connect(std::string ip_addr, int port) {
    uint8_t buf[2048];
    dst_addr.sin_addr.s_addr = inet_addr(ip_addr.c_str());
    dst_addr.sin_port = htons(port);

    // 第一次握手
    uint16_t flags = TCP_FLAG_SYN;
    uint32_t sender_seq = std::rand();

    std::vector<uint8_t> seg = make_segment(sender_seq, 0, flags, 0);
    send_seg(seg.data(), seg.size());

    status = Status::SYN_SENT;

    // 第三次握手
    // 解析数据包
    int n = recvfrom(sender_fd, buf, sizeof(buf), 0, nullptr, nullptr);
    if (n <= 0)
        return -1;

    int ihl = (buf[0] & 0x0F) * 4;
    MYHDR *hdr = reinterpret_cast<MYHDR *>(buf + ihl);

    uint32_t recv_seq = ntohl(hdr->seq);
    uint32_t recv_ack = ntohl(hdr->ack);
    uint16_t recv_flags = ntohs(hdr->flags);

    if (status == Status::SYN_SENT) {
        // 判断第二次握手的合法性
        if ((recv_flags & (TCP_FLAG_SYN | TCP_FLAG_ACK)) ==
                (TCP_FLAG_SYN | TCP_FLAG_ACK) &&
            recv_ack == sender_seq + 1) {

            uint16_t ack_flag = TCP_FLAG_ACK;

            sender_seq += 1;
            uint32_t my_ack = recv_seq + 1;

            std::vector<uint8_t> ack_seg =
                make_segment(sender_seq, my_ack, ack_flag, 0);

            send_seg(ack_seg.data(), ack_seg.size());

            status = Status::ESTABLISHED;
        }
    }

    return 0;
}

int Sender::disconnect() {
    if (dst_addr.sin_port == 0) {
        return 1;
    }
}

int main() {
    Sender sender1(3455);
    sender1.connect("127.0.0.1", 3456);
    return 0;
}