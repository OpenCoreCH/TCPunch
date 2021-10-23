#ifndef HOLEPUNCHINGSERVERCLIENT_UTILS_H
#define HOLEPUNCHINGSERVERCLIENT_UTILS_H

#include <arpa/inet.h>
#include <exception>
#include "../client/tcpunch.h"

typedef struct {
    struct in_addr ip;
    in_port_t      port;
} PeerConnectionData;

void error_exit(const std::string& error_string) {
    throw error_string;
}

void error_exit_errno(const std::string& error_string) {
    if (errno == EAGAIN) {
        throw Timeout();
    } else {
        std::string err = error_string + strerror(errno);
        throw err;
    }
}

std::string ip_to_string(in_addr_t *ip) {
    char str_buffer[20];
    inet_ntop(AF_INET, ip, str_buffer, sizeof(str_buffer));
    return {str_buffer};
}

#endif //HOLEPUNCHINGSERVERCLIENT_UTILS_H
