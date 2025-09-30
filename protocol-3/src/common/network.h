#ifndef NETWORK_H
#define NETWORK_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

// 初始化 Winsock
bool initialize_winsock(WSADATA &wsaData);

// 创建套接字
SOCKET create_socket();

// 设置服务器地址
void setup_server_address(sockaddr_in &server_addr, int port, const char* ip = "127.0.0.1");

#endif // NETWORK_H
