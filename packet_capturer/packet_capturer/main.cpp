#include <pcap.h>
#include <iostream>
#include <string>
#include <vector>
#include <iomanip> // ���ڸ�ʽ�������ͷ�ļ�
#include <winsock2.h> // Windows �ض���ͷ�ļ�
#include <ws2tcpip.h> // Windows �ض���ͷ�ļ�

#pragma comment(lib, "ws2_32.lib") // ���� Winsock ��

#pragma pack(1)        // �����ֽڶ��뷽ʽ

// ֡�ײ��ṹ
typedef struct FrameHeader_t {
    u_char DesMAC[6];  // Ŀ�ĵ�ַ
    u_char SrcMAC[6];  // Դ��ַ
    u_short FrameType; // ֡����
} FrameHeader_t;

// IP�ײ��ṹ
typedef struct IPHeader_t {
    u_char Ver_HLen;   // �汾 (4 bits) + �ײ����� (4 bits)
    u_char TOS;        // ��������
    u_short TotalLen;  // �ܳ���
    u_short ID;        // ��ʶ
    u_short Flag_Segment; // ��־ (3 bits) + Ƭƫ�� (13 bits)
    u_char TTL;        // ����ʱ��
    u_char Protocol;   // Э��
    u_short Checksum;  // �ײ�У���
    u_long SrcIP;      // Դ��ַ
    u_long DstIP;      // Ŀ�ĵ�ַ
} IPHeader_t;

// ����֡�ײ���IP�ײ������ݰ��ṹ
typedef struct Data_t {
    FrameHeader_t FrameHeader;
    IPHeader_t IPHeader;
} Data_t;

// �ָ�Ĭ�϶��뷽ʽ
#pragma pack()

class PacketCapturer {
  public:
    PacketCapturer() : handle(nullptr), alldevs(nullptr), device(nullptr) {}

    ~PacketCapturer() {
        if (handle) {
            pcap_close(handle);
        }
        if (alldevs) {
            pcap_freealldevs(alldevs);
        }
    }

    // ѡ���豸
	bool selectDevice() {   
        char errbuf[PCAP_ERRBUF_SIZE];

        // �������п��õ������豸
        if (pcap_findalldevs(&alldevs, errbuf) == -1) {
            std::cerr << "pacp�����豸���ִ���: " << errbuf << std::endl;
            return false;
        }

        // �г������豸
        int i = 0;
        for (device = alldevs; device != nullptr; device = device->next) {
            std::cout << ++i << ": " << device->name;
            if (device->description) {
                std::cout << " (" << device->description << ")";
            }
            std::cout << std::endl;
            device_names.push_back(device->name);
        }

        if (i == 0) {
            std::cerr << "û���ҵ��豸" << std::endl;
            return false;
        }

        // ���û�ѡ��һ���豸
        int choice;
        std::cout << "������Ҫѡ����豸�����ֱ��: ";
        std::cin >> choice;

        if (choice < 1 || choice > i) {
            std::cerr << "��Чѡ��" << std::endl;
            return false;
        }

        // ѡ���û�ָ�����豸
        device = alldevs;
        for (int index = 1; index < choice; ++index) {
            device = device->next;
        }

        if (device != nullptr) {
            std::cout << "ʹ�õ��豸��: " << device->name << std::endl;
            return true;
        }
        else {
            std::cerr << "�豸ָ��Ϊ�ա�" << std::endl;
            return false;
        }
    }

    // �������ݰ�
    bool capturePackets(int packet_count = -1) {
        char errbuf[PCAP_ERRBUF_SIZE];

        // ���豸���в���
        handle = pcap_open_live(device->name, BUFSIZ, 1, 1000, errbuf);
        if (handle == nullptr) {
            std::cerr << "�޷����豸 " << device->name << ": "
                      << errbuf << std::endl;
            return false;
        }

        // �������ݰ�
        if (pcap_loop(handle, packet_count, packetHandler,
                      reinterpret_cast<u_char *>(this)) < 0) {
            std::cerr << "pcap_loop���ִ���: " << pcap_geterr(handle)
                      << std::endl;
            return false;
        }

        return true;
    }

    // �������ݰ�
    void analyzePacket(const struct pcap_pkthdr* header, const u_char* packet) {
        // ����֡�ײ�
        FrameHeader_t* frame_header = (FrameHeader_t*)packet;

        // ���֡�����Ƿ�ΪIP
        if (ntohs(frame_header->FrameType) == 0x0800) {
            // ����IPͷ
            IPHeader_t* ip_header = (IPHeader_t*)(packet + sizeof(FrameHeader_t));

            // ��ȡԴ��ַ��Ŀ���ַ
            char src_ip[INET_ADDRSTRLEN];
            char dst_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(ip_header->SrcIP), src_ip, INET_ADDRSTRLEN);
            inet_ntop(AF_INET, &(ip_header->DstIP), dst_ip, INET_ADDRSTRLEN);

            // ���Դ��ַ��Ŀ���ַ��ʹ��setw��left����ʽ�����
            std::cout << std::left << std::setw(20) << "ԴIP: " << std::setw(20) << src_ip
                << " | "
                << std::left << std::setw(20) << "Ŀ��IP: " << std::setw(20) << dst_ip
                << std::endl;
        }
    }

  private:
	  // ���ݰ�����
    static void packetHandler(u_char *user, const struct pcap_pkthdr *header,
                              const u_char *packet) {
        PacketCapturer *capturer = reinterpret_cast<PacketCapturer *>(user);
        capturer->analyzePacket(header, packet);
    }

	pcap_t* handle; // ���ڲ������ݰ��ľ��
	pcap_if_t* alldevs; // �豸�б�
	pcap_if_t* device; // ѡ����豸
	std::vector<std::string> device_names; // �豸�����б�
};

int main() {
    PacketCapturer capturer;

    if (!capturer.selectDevice()) {
        return 1;
    }

    if (!capturer.capturePackets()) {
        return 1;
    }

    return 0;
}