cmake_minimum_required(VERSION 2.8)

project(tcpunchd CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(tcpunchd hole_punching_server.cpp)
