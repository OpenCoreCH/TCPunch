#include "./hole_punching_client.h"

using boost::asio::ip::udp;
using boost::asio::ip::address;

udp::endpoint pair(udp::socket& socket, std::string pairing_name, std::string server_address) {
	const std::size_t buffer_length = sizeof(in_addr) + sizeof(in_port_t);
	char buffer[buffer_length];

    udp::endpoint punching_server = udp::endpoint(address::from_string(server_address), SERVER_PORT);
    socket.open(udp::v4());

    boost::system::error_code err;
    auto sent = socket.send_to(boost::asio::buffer(pairing_name), punching_server, 0, err);

	int n = socket.receive_from(boost::asio::buffer(buffer), punching_server);
	in_addr* peer_address = (in_addr*) buffer;
	std::string peer_ip(inet_ntoa(*peer_address));
    in_port_t* peer_port = (in_port_t*) (buffer + sizeof(in_addr));
	unsigned short peer_port_h = ntohs(*peer_port);  // Need to convert port from network byte order to host byte order
	std::cout << "Peer IP: " << peer_ip << std::endl;
    std::cout << "Peer Port: " << *peer_port << std::endl;


	udp::endpoint peer = udp::endpoint(address::from_string(peer_ip), peer_port_h);
	return peer;
}