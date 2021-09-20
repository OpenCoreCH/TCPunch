#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <iostream>

#define SERVER_PORT 10000

#if __APPLE__
    #define MSG_CONFIRM 0
#endif

int main() {
	int sockfd;
    int buffer_length = sizeof(in_addr) + sizeof(in_port_t);
	char buffer[buffer_length];
	std::string pairingName("Test");
	std::string serverAddress("192.168.0.198");
	struct sockaddr_in servaddr;

	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, serverAddress.c_str(), &(servaddr.sin_addr.s_addr));
	
	int n, len;

	
	sendto(sockfd, pairingName.c_str(), pairingName.length(), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));

	n = recvfrom(sockfd, (char *)buffer, buffer_length, MSG_WAITALL, (struct sockaddr *) &servaddr, (socklen_t *) &len);
    in_addr* peer_address = (in_addr*) buffer;
    in_port_t* peer_port = (in_port_t*) (buffer + sizeof(in_addr));
	printf("Peer IP : %s\n", inet_ntoa(*peer_address));
    printf("Peer Port: %hu\n", *peer_port);

	struct sockaddr_in peeraddr;
	peeraddr.sin_family = AF_INET;
	peeraddr.sin_port = *peer_port;
	peeraddr.sin_addr = *peer_address;
	char recv_buffer[128];
	sendto(sockfd, "Ping", 4, MSG_CONFIRM, (const struct sockaddr *) &peeraddr, sizeof(peeraddr));
	n = recvfrom(sockfd, recv_buffer, 128, MSG_WAITALL, (struct sockaddr *) &peeraddr, (socklen_t *) &len);
	recv_buffer[n] = '\0';
	printf("%s", recv_buffer);

	close(sockfd);
	return 0;
}

