#include "tcpunch.h"
#include <fcntl.h>
#include <csignal>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <cerrno>
#include <string>
#include <chrono>
#include <thread>
#include <stdexcept>
#include <iostream>
#include <atomic>
#include "../common/utils.h"


std::atomic<bool> connection_established(false);
std::atomic<int> accepting_socket(-1);

void* peer_listen(void* p) {
    auto* info = (PeerConnectionData*)p;

    // Create socket on the port that was previously used to contact the rendezvous server
    int listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket == -1) {
        error_exit_errno("Socket creation failed: ");
    }
    int enable_flag = 1;
    if (setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &enable_flag, sizeof(int)) < 0 ||
        setsockopt(listen_socket, SOL_SOCKET, SO_REUSEPORT, &enable_flag, sizeof(int)) < 0) {
        error_exit_errno("Setting REUSE options failed: ");
    }

    struct sockaddr_in local_port_data{};
    local_port_data.sin_family = AF_INET;
    local_port_data.sin_addr.s_addr = INADDR_ANY;
    local_port_data.sin_port = info->port;

    if (bind(listen_socket, (const struct sockaddr *)&local_port_data, sizeof(local_port_data)) < 0) {
        error_exit_errno("Could not bind to local port: ");
    }

    if (listen(listen_socket, 1) == -1) {
        error_exit_errno("Listening on local port failed: ");
    }

    struct sockaddr_in peer_info{};
    unsigned int len = sizeof(peer_info);

    while(true) {
        int peer = accept(listen_socket, (struct sockaddr*)&peer_info, &len);
        if (peer == -1) {
#if DEBUG
            std::cout << "Error when connecting to peer" << strerror(errno) << std::endl;
#endif
        } else {
#if DEBUG
            std::cout << "Succesfully connected to peer, accepting" << std::endl;
#endif
            accepting_socket = peer;
            connection_established = true;
            return 0;
        }
    }
}

int pair(const std::string& pairing_name, const std::string& server_address, int port, int timeout_ms) {
    connection_established = false;
    accepting_socket = -1;
    struct timeval timeout;
    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;

    int socket_rendezvous;
    struct sockaddr_in server_data{};

    socket_rendezvous = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_rendezvous == -1) {
        error_exit_errno("Could not create socket for rendezvous server: ");
    }

    // Enable binding multiple sockets to the same local endpoint, see https://bford.info/pub/net/p2pnat/ for details
    int enable_flag = 1;
    if (setsockopt(socket_rendezvous, SOL_SOCKET, SO_REUSEADDR, &enable_flag, sizeof(int)) < 0 ||
        setsockopt(socket_rendezvous, SOL_SOCKET, SO_REUSEPORT, &enable_flag, sizeof(int)) < 0) {
        error_exit_errno("Setting REUSE options failed: ");
    }
    if (setsockopt(socket_rendezvous, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof timeout) < 0 ||
        setsockopt(socket_rendezvous, SOL_SOCKET, SO_REUSEPORT, &enable_flag, sizeof(int)) < 0) {
        error_exit_errno("Setting timeout failed: ");
    }

    server_data.sin_family = AF_INET;
    server_data.sin_addr.s_addr = inet_addr(server_address.c_str());
    server_data.sin_port = htons(port);

    if (connect(socket_rendezvous, (struct sockaddr *)&server_data, sizeof(server_data)) != 0) {
        error_exit_errno("Connection with the rendezvous server failed: ");
    }

    if(send(socket_rendezvous, pairing_name.c_str(), pairing_name.length(), MSG_DONTWAIT) == -1) {
        error_exit_errno("Failed to send data to rendezvous server: ");
    }

    PeerConnectionData public_info;
    ssize_t bytes = recv(socket_rendezvous, &public_info, sizeof(public_info), MSG_WAITALL);
    if (bytes == -1) {
        error_exit_errno("Failed to get data from rendezvous server: ");
    } else if(bytes == 0) {
        error_exit("Server has disconnected");
    }

    pthread_t peer_listen_thread;
    int thread_return = pthread_create(&peer_listen_thread, nullptr, peer_listen, (void*) &public_info);
    if(thread_return) {
        error_exit_errno("Error when creating thread for listening: ");
    }

    PeerConnectionData peer_data;

    // Wait until rendezvous server sends info about peer
    ssize_t bytes_received = recv(socket_rendezvous, &peer_data, sizeof(peer_data), MSG_WAITALL);
    if(bytes_received == -1) {
        error_exit_errno("Failed to get peer data from rendezvous server: ");
    } else if(bytes_received == 0) {
        error_exit("Server has disconnected when waiting for peer data");
    }
#if DEBUG
    std::cout << "Peer: " << ip_to_string(&peer_data.ip.s_addr) << ":" << ntohs(peer_data.port) << std::endl;
#endif

    //We do NOT close the socket_rendezvous socket here, otherwise the next binds sometimes fail (although SO_REUSEADDR|SO_REUSEPORT is set)!

    int peer_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (setsockopt(peer_socket, SOL_SOCKET, SO_REUSEADDR, &enable_flag, sizeof(int)) < 0 ||
        setsockopt(peer_socket, SOL_SOCKET, SO_REUSEPORT, &enable_flag, sizeof(int)) < 0) {
        error_exit("Setting REUSE options failed");
    }

    //Set socket to non blocking for the following polling operations
    if(fcntl(peer_socket, F_SETFL, O_NONBLOCK) != 0) {
        error_exit_errno("Setting O_NONBLOCK failed: ");
    }

    struct sockaddr_in local_port_addr = {0};
    local_port_addr.sin_family = AF_INET;
    local_port_addr.sin_addr.s_addr = INADDR_ANY;
    local_port_addr.sin_port = public_info.port;

    if (bind(peer_socket, (const struct sockaddr *)&local_port_addr, sizeof(local_port_addr))) {
        error_exit_errno("Binding to same port failed: ");
    }

    struct sockaddr_in peer_addr = {0};
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_addr.s_addr = peer_data.ip.s_addr;
    peer_addr.sin_port = peer_data.port;

    while(!connection_established.load()) {
        int peer_status = connect(peer_socket, (struct sockaddr *)&peer_addr, sizeof(struct sockaddr));
        if (peer_status != 0) {
            if (errno == EALREADY || errno == EAGAIN || errno == EINPROGRESS) {
                continue;
            } else if(errno == EISCONN) {
                #if DEBUG
                std::cout << "Succesfully connected to peer, EISCONN" << std::endl;
                #endif
                break;
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
        } else {
            #if DEBUG
            std::cout << "Succesfully connected to peer, peer_status" << std::endl;
            #endif
            break;
        }
    }

    if(connection_established.load()) {
        pthread_join(peer_listen_thread, nullptr);
        peer_socket = accepting_socket.load();
    }

    int flags = fcntl(peer_socket,  F_GETFL, 0);
    flags &= ~(O_NONBLOCK);
    fcntl(peer_socket, F_SETFL, flags);

    return peer_socket;
}