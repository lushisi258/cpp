#ifndef FILE_RECEIVER_H
#define FILE_RECEIVER_H

#include "../common/network.h"
#include "../common/packet.h"
#include <fstream>

void receive_file(SOCKET sockfd, sockaddr_in &client_addr, int addr_len,
                  const char *output_filename);

char *get_file_name(sockaddr_in &client_addr, SOCKET sockfd);

bool server_establish_connection(SOCKET sockfd, sockaddr_in &client_addr,
                                 int addr_len);

int server_close_connection(SOCKET sockfd, sockaddr_in &client_addr,
                            int addr_len);

#endif // FILE_RECEIVER_H
