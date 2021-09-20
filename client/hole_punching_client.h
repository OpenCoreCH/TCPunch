#ifndef HOLE_PUNCHING_CLIENT_H
#define HOLE_PUNCHING_CLIENT_H
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#define SERVER_PORT 10000
#define MAX_BUFFER_DATA_SIZE 65500

boost::asio::ip::udp::endpoint pair(boost::asio::ip::udp::socket& socket, std::string pairing_name, std::string server_address);

#endif