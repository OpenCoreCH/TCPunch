cmake_minimum_required(VERSION 3.10)

project(HolePunchingClient VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(hole_punching_client SHARED hole_punching_client.cpp)
target_link_libraries(hole_punching_client PRIVATE Threads::Threads)