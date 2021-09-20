#include "./hole_punching_client.h"

int pair(struct sockaddr_in &peeraddr, std::string pairing_name, std::string server_address) {
	int sockfd;
	struct sockaddr_in servaddr;
	
	const std::size_t buffer_length = sizeof(in_addr) + sizeof(in_port_t);
	char buffer[buffer_length];

    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, server_address.c_str(), &(servaddr.sin_addr.s_addr));
	
	int n, len;

	
	sendto(sockfd, pairing_name.c_str(), pairing_name.length(), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));

	n = recvfrom(sockfd, (char *)buffer, buffer_length, MSG_WAITALL, (struct sockaddr *) &servaddr, (socklen_t *) &len);
	
	peeraddr.sin_family = AF_INET;
	peeraddr.sin_addr = *((in_addr*) buffer);
    peeraddr.sin_port = *((in_port_t*) (buffer + sizeof(in_addr)));
	printf("Peer IP : %s\n", inet_ntoa(peeraddr.sin_addr));
    printf("Peer Port: %hu\n", peeraddr.sin_port);

	return sockfd;
}