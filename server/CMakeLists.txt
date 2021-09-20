cmake_minimum_required(VERSION 3.10)

project(HolePunchingServer VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(hole_punching_server hole_punching_server.cpp)