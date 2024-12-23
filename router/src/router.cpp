#include "../include/router.h"

// Router::Router() {
//     // 初始化路由表
//     // 从router.conf文件中读取路由表信息
//     std::ifstream file("../data/router.conf");
//     if (!file.is_open()) {
//         std::cerr << "无法打开router.conf文件" << std::endl;
//         return;
//     } else {
//         std::string line;
//         while (std::getline(file, line)) {
//             std::string destination, gateway, netmask, iface;
//             std::istringstream iss(line);
//             iss >> destination >> gateway >> netmask >> iface;
//             add_route(destination, gateway, netmask, iface);
//         }
//     }
// }

class Router {
public:
    void start() {
        running = true;
        std::thread receive_thread(&Router::capture_packets, this, -1);
        std::thread forward_thread(&Router::forward_packets, this);

        receive_thread.detach();
        forward_thread.detach();
    }

    int capture_packets(int packet_count = -1) {
        char errbuf[PCAP_ERRBUF_SIZE];

        // 打开设备进行捕获
        handle = pcap_open_live(device->name, BUFSIZ, 1, 1000, errbuf);
        if (handle == nullptr) {
            std::cerr << "无法打开设备 " << device->name << ": " << errbuf << std::endl;
            return 1;
        }

        // 捕获数据包
        if (pcap_loop(handle, packet_count, packet_handler, nullptr) < 0) {
            std::cerr << "数据包捕获出现错误: " << pcap_geterr(handle) << std::endl;
            return 2;
        }

        return 0;
    }

    void forward_packets() {
        while (running) {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this] { return !running || !packet_queue.empty(); });
            if (!running && packet_queue.empty()) {
                break;
            }
            // 转发数据包的代码
            auto packet = packet_queue.front();
            packet_queue.pop();
        }
    }

private:
    bool running;
    pcap_t* handle;
    std::mutex mtx;
    std::condition_variable cv;
    std::queue<const u_char*> packet_queue;
    // 假设 device 是一个指向设备信息的指针
    struct Device {
        const char* name;
    } *device;

    static void packet_handler(u_char* user, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
        // 数据包处理代码
    }
};

// void Router::start() {
//         running = true;
//         std::thread receive_thread(&Router::capture_packets, this, -1);
//         std::thread forward_thread(&Router::forward_packets, this);

//         receive_thread.detach();
//         forward_thread.detach();
//     }

// void Router::stop() {
//     running = false;
//     cv.notify_all();
//     if (receive_thread.joinable()) {
//         receive_thread.join();
//     }
//     if (forward_thread.joinable()) {
//         forward_thread.join();
//     }
// }

// int Router::select_device() {
//     char errbuf[PCAP_ERRBUF_SIZE];
//     std::vector<std::string> device_names;

//     // 查找所有可用的网络设备
//     if (pcap_findalldevs(&alldevs, errbuf) == -1) {
//         std::cerr << "pcap查找设备出现错误: " << errbuf << std::endl;
//         return 1;
//     }

//     // 列出所有设备
//     int i = 0;
//     for (device = alldevs; device != nullptr; device = device->next) {
//         std::cout << ++i << ": " << device->name;
//         if (device->description) {
//             std::cout << " (" << device->description << ")";
//         }
//         std::cout << std::endl;
//         device_names.push_back(device->name);
//     }

//     if (i == 0) {
//         std::cerr << "没有找到设备" << std::endl;
//         pcap_freealldevs(alldevs);
//         return 2;
//     }

//     // 让用户选择一个设备
//     int choice;
//     std::cout << "输入你要选择的设备的数字编号: ";
//     std::cin >> choice;

//     if (choice < 1 || choice > i) {
//         std::cerr << "无效选择" << std::endl;
//         pcap_freealldevs(alldevs);
//         return 3;
//     }

//     // 选择用户指定的设备
//     device = alldevs;
//     for (int index = 1; index < choice; ++index) {
//         device = device->next;
//     }

//     if (device != nullptr) {
//         std::cout << "使用的设备是: " << device->name << std::endl;

//         // 获取设备的IP地址并存入列表
//         for (pcap_addr_t *addr = device->addresses; addr != nullptr; addr = addr->next) {
//             if (addr->addr->sa_family == AF_INET) {
//                 struct sockaddr_in *sockaddr = (struct sockaddr_in *)addr->addr;
//                 u_long ip = sockaddr->sin_addr.s_addr;
//                 device_ip.push_back(ip);
//                 std::cout << "IP地址: " << inet_ntoa(sockaddr->sin_addr) << std::endl;
//             }
//         }

//         pcap_freealldevs(alldevs);
//         return 0;
//     } else {
//         std::cerr << "设备指针为空。" << std::endl;
//         pcap_freealldevs(alldevs);
//         return 4;
//     }
// }

// int Router::capture_packets(int packet_count = -1) {
//     char errbuf[PCAP_ERRBUF_SIZE];

//     // 打开设备进行捕获
//     handle = pcap_open_live(device->name, BUFSIZ, 1, 1000, errbuf);
//     if (handle == nullptr) {
//         std::cerr << "无法打开设备 " << device->name << ": " << errbuf
//                   << std::endl;
//         return 1;
//     }

//     // 捕获数据包
//     if (pcap_loop(handle, packet_count, packet_handler, nullptr) < 0) {
//         std::cerr << "数据包捕获出现错误: " << pcap_geterr(handle)
//                   << std::endl;
//         return 2;
//     }

//     return 0;
// }

// void Router::packet_handler(u_char *user, const struct pcap_pkthdr *header, const u_char *packet) {
//     Router *router = reinterpret_cast<Router *>(user);
//     router->handle_packet(packet);
// }

// void Router::handle_packet(const u_char *packet) {
//     // 输出包的基本信息
//     struct FrameHeader_t *frameHeader = (struct FrameHeader_t *)packet;
//     std::cout << "帧类型: " << frameHeader->FrameType << std::endl;
// }

// void Router::forward_packets(const u_char *packet) {
//     while (running) {
//         std::unique_lock<std::mutex> lock(mtx);
//         cv.wait(lock, [this] { return !running || !packet_queue.empty(); });
//         if (!running && packet_queue.empty()) {
//             break;
//         }
//         // 转发数据包的代码
//         auto packet = packet_queue.front();
//         packet_queue.pop();
//     }
// }

// void Router::add_route(const std::string &destination, const std::string &gateway,
//                        const std::string &netmask, const std::string &iface) {
//     // 添加路由的代码
// }

// RouteEntry *Router::find_route(const std::string &destination) {
//     // 查找路由的代码
//     return nullptr;
// }

// int Router::forward() {
//     // 转发数据包的代码
//     return 0;
// }

// Router::~Router() {
//     if (receive_thread.joinable()) {
//         receive_thread.join();
//     }
//     if (forward_thread.joinable()) {
//         forward_thread.join();
//     }
// }