#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <map>
#include <string>
#include <cstring>
  
#define PORT     10000
#define MAXPAIRINGNAME 100

#if __APPLE__
    #define MSG_CONFIRM 0
#endif
  
int main() {
    int sockfd;
    char buffer[MAXPAIRINGNAME];
    struct sockaddr_in servaddr, clientaddr;
      
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("Could not create socket file descriptor");
        exit(EXIT_FAILURE);
    }
      
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&clientaddr, 0, sizeof(clientaddr));
      
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);
      
    // Bind the socket with the server address
    if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
      
    int len, n;
  
    len = sizeof(clientaddr);

    // Maps pairing names to sockaddrs (IP & port)
    std::map<std::string, sockaddr_in> client_connections;

    while (true) {
        n = recvfrom(sockfd, (char *)buffer, MAXPAIRINGNAME, MSG_WAITALL, ( struct sockaddr *) &clientaddr, (socklen_t*) &len);
        buffer[n] = '\0';
        printf("Got pairing request with name : %s\n", buffer);
        std::string pairing_name(buffer);
        auto existing_entry = client_connections.find(pairing_name);
        if (existing_entry != client_connections.end()) {
            int buffer_length = sizeof(in_addr) + sizeof(in_port_t);
            char send_buffer_b[buffer_length];
            std::memcpy(send_buffer_b, &(existing_entry->second.sin_addr), sizeof(in_addr));
            std::memcpy(send_buffer_b + sizeof(in_addr), &(existing_entry->second.sin_port), sizeof(in_port_t));
            sendto(sockfd, send_buffer_b, buffer_length, MSG_CONFIRM, (const struct sockaddr *) &clientaddr, len);

            char send_buffer_a[buffer_length];
            std::memcpy(send_buffer_a, &(clientaddr.sin_addr), sizeof(in_addr));
            std::memcpy(send_buffer_a + sizeof(in_addr), &(clientaddr.sin_port), sizeof(in_port_t));
            sendto(sockfd, send_buffer_a, buffer_length, MSG_CONFIRM, (const struct sockaddr *) &(existing_entry->second), len);

            printf("Replied with address.\n");
            client_connections.erase(pairing_name);
        } else {
            client_connections[pairing_name] = clientaddr;    
        }
        
    }
      
    return 0;
}