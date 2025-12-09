#include "./include/segment.h"
#include <arpa/inet.h>
#include <cstring>
#include <fstream>
#include <sys/socket.h>

class Sender {
    int connect(std::string ip_addr);
    void send_file(std::string dir_path, std::string file_name);
};