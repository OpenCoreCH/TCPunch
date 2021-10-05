#ifndef HOLE_PUNCHING_CLIENT_H
#define HOLE_PUNCHING_CLIENT_H
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cstring>
#include <arpa/inet.h>

#define DEBUG 0
static const int MAX_RECV_BUFFER = 2048;


int pair(const std::string& pairing_name, const std::string& server_address, int port = 10000);

#endif