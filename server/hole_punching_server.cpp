#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cerrno>
#include <string>
#include <map>
#include <iostream>
#include "../common/utils.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
#define DEFAULT_LISTEN_PORT 10000

typedef struct {
    int socket;
    struct sockaddr_in client_info;	
} ConnectionData;

static const size_t MAX_PAIRING_NAME = 100;
std::map<std::string, ConnectionData> clients;

int main(int argc, char** argv) {
    int listen_port = DEFAULT_LISTEN_PORT;
    if (argc == 2) {
        char *p;
        long input = strtol(argv[1], &p, 10);
        if (errno != 0 || *p != '\0' || input > 65535 || input < 1) {
            std::string usage_string("Usage: ");
            usage_string += argv[0];
            usage_string += " <port>";
            error_exit(usage_string);
        } else {
            listen_port = (int) input;
        }
    }
    int server_socket;
    struct sockaddr_in server_data{};
    server_socket = socket(AF_INET, SOCK_STREAM , 0);

    if (server_socket == -1) {
        error_exit_errno("Error when creating socket: ");
    }
    
    int enable_flag = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &enable_flag, sizeof(int)) < 0) {
        error_exit_errno("Setting REUSEADDR failed: ");
    }

    server_data.sin_family = AF_INET;
    server_data.sin_addr.s_addr = INADDR_ANY;
    server_data.sin_port = htons(listen_port);

    if (bind(server_socket, (const struct sockaddr *)&server_data, 16) < 0) {
        error_exit_errno("Error on bind: ");
    }

    if (listen(server_socket, 255) == -1) {
        error_exit_errno("Listening failed: ");
    }
    std::cout << "Listening for connections on port: " << listen_port << std::endl;

    while(true) {
        struct sockaddr_in client_data{};
        unsigned int len = sizeof(client_data);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_data, &len);

        if (client_socket < 0) {
            error_exit_errno("Accepting clients failed: ");
        }



        char buffer[MAX_PAIRING_NAME] = {0};
        char client_msg_buffer[MAX_PAIRING_NAME] = {0};
        int n = recv(client_socket, (void*)client_msg_buffer, MAX_PAIRING_NAME, 0);
        std::string pairing_name = std::string(client_msg_buffer);
        if (n == 0) {
            std::cout << "Client has disconnected" << std::endl;
        } else if (n == -1) {
            std::cout << "Recv failed: " << strerror(errno) << std::endl;
        } else {
            std::cout << "Connection request from client with pairing name: " << pairing_name << std::endl;
        }

        PeerConnectionData info;
        info.ip = client_data.sin_addr;
        info.port = client_data.sin_port;
        memcpy(buffer, &info, sizeof(info));

        if (send(client_socket, buffer, sizeof(info), 0) > 0) {
            std::cout << "Replied to client with pairing name: " << pairing_name << std::endl;
        } else {
            std::cout << "Error when replying: " << strerror(errno) << std::endl;
        }


        auto existing_entry = clients.find(pairing_name);
        if (existing_entry != clients.end()) {
            // First client with same pairing name already connected, reply to both
            PeerConnectionData info1;
            info1.ip = client_data.sin_addr;
            info1.port = client_data.sin_port;

            if(send(clients[pairing_name].socket, &info1, sizeof(info1), 0) > 0) {
                std::cout << pairing_name << ", reply 1: " << std::endl;
                std::cout << ip_to_string(&info1.ip.s_addr) << ":" << ntohs(info1.port) << std::endl;
            } else {
                std::cout << "Error when sending reply 1: " << strerror(errno) << std::endl;
            }
            close(clients[pairing_name].socket);

            PeerConnectionData info2;
            // Client 1 receives client 0 info
            info2.ip = clients[pairing_name].client_info.sin_addr;
            info2.port = clients[pairing_name].client_info.sin_port;

            if(send(client_socket, &info2, sizeof(info2), 0) > 0) {
                std::cout << pairing_name << ", reply 2: " << std::endl;
                std::cout << ip_to_string(&info2.ip.s_addr) << ":" << ntohs(info2.port) << std::endl;
            } else {
                std::cout << "Error when sending reply 2: " << strerror(errno) << std::endl;
            }
            close(client_socket);

            clients.erase(existing_entry);
        } else {
            ConnectionData client;
            client.socket = client_socket;
            client.client_info = client_data;
            clients[pairing_name] = client;
        }
    }

    return 0;
}
#pragma clang diagnostic pop