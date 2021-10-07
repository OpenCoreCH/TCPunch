#ifndef HOLE_PUNCHING_CLIENT_H
#define HOLE_PUNCHING_CLIENT_H
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cstring>
#include <arpa/inet.h>
#include <exception>

#define DEBUG 0

struct Timeout : public std::exception {};

int pair(const std::string& pairing_name, const std::string& server_address, int port = 10000, int timeout_ms = 0);

#endif