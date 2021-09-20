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
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>

#define SERVER_PORT 10000
#define MAX_BUFFER_DATA_SIZE 65527

#if __APPLE__
    #define MSG_CONFIRM 0
#endif

using boost::asio::ip::udp;
using boost::asio::ip::address;

int main() {
	std::string pairingName("Test");
	std::string serverAddress("192.168.0.198");
	const std::size_t buffer_length = sizeof(in_addr) + sizeof(in_port_t);
	char buffer[buffer_length];

	boost::asio::io_service io_service;
    udp::socket socket(io_service);
    udp::endpoint punching_server = udp::endpoint(address::from_string(serverAddress), SERVER_PORT);
    socket.open(udp::v4());

    boost::system::error_code err;
    auto sent = socket.send_to(boost::asio::buffer(pairingName), punching_server, 0, err);

	int n = socket.receive_from(boost::asio::buffer(buffer), punching_server);
	in_addr* peer_address = (in_addr*) buffer;
	std::string peer_ip(inet_ntoa(*peer_address));
    in_port_t* peer_port = (in_port_t*) (buffer + sizeof(in_addr));
	unsigned short peer_port_h = ntohs(*peer_port);  // Need to convert port from network byte order to host byte order
	std::cout << "Peer IP: " << peer_ip << std::endl;
    std::cout << "Peer Port: " << *peer_port << std::endl;


	udp::endpoint peer = udp::endpoint(address::from_string(peer_ip), peer_port_h);
	socket.send_to(boost::asio::buffer("Ping"), peer, 0, err);
	char recv_buffer[MAX_BUFFER_DATA_SIZE];
	socket.receive_from(boost::asio::buffer(recv_buffer), peer);
	std::cout << recv_buffer << std::endl;
    
	return 0;
}

