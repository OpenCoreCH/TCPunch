#ifndef HOLE_PUNCHING_CLIENT_H
#define HOLE_PUNCHING_CLIENT_H
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cstring>
#include <arpa/inet.h>

#if __APPLE__
    #define MSG_CONFIRM 0
#endif


#define SERVER_PORT 10000
#define MAX_BUFFER_DATA_SIZE 65500

int pair(struct sockaddr_in &peeraddr, std::string pairing_name, std::string server_address);

#endif