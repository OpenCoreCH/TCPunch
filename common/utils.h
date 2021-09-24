#ifndef HOLEPUNCHINGSERVERCLIENT_UTILS_H
#define HOLEPUNCHINGSERVERCLIENT_UTILS_H

#include <arpa/inet.h>

typedef struct {
    struct in_addr ip;
    in_port_t      port;
} PeerConnectionData;

void error_exit(const std::string& error_string) {
    std::cerr << error_string << std::endl;
    exit(EXIT_FAILURE);
}

void error_exit_errno(const std::string& error_string) {
    std::cerr << error_string << strerror(errno) << std::endl;
    exit(EXIT_FAILURE);
}

std::string ip_to_string(in_addr_t *ip) {
    char str_buffer[20];
    inet_ntop(AF_INET, ip, str_buffer, sizeof(str_buffer));
    return {str_buffer};
}

#endif //HOLEPUNCHINGSERVERCLIENT_UTILS_H
