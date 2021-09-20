cmake_minimum_required(VERSION 3.10)

project(HolePunchingClient VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(hole_punching_client hole_punching_client.cpp)
target_link_libraries(hole_punching_client PRIVATE Threads::Threads)