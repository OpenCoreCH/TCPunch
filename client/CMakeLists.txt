cmake_minimum_required(VERSION 3.10)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)


project(HolePunchingClient VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Threads REQUIRED)
add_library(hole_punching_client SHARED hole_punching_client.cpp)
target_link_libraries(hole_punching_client PRIVATE Threads::Threads)

#add_executable(tester tester.cpp)
#target_link_libraries(tester PRIVATE hole_punching_client)
