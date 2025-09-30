#ifndef FILE_SENDER_H
#define FILE_SENDER_H

#include "../common/network.h"
#include "../common/packet.h"
#include <fstream>
#include <chrono>

void send_file(SOCKET sockfd, sockaddr_in &server_addr, int addr_len, const char *filename);

void send_file_name(SOCKET sockfd, sockaddr_in &server_addr, int addr_len, const char *filename);

bool client_establish_connection(SOCKET sockfd, sockaddr_in &server_addr, int addr_len);

int close_connection(SOCKET sockfd, sockaddr_in &server_addr, int addr_len);

#endif // FILE_SENDER_H
