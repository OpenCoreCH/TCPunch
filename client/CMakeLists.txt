cmake_minimum_required(VERSION 3.10)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


project(tcpunch VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)



find_package(Threads REQUIRED)
add_library(tcpunch STATIC tcpunch.cpp)
target_include_directories(tcpunch PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(tcpunch PRIVATE Threads::Threads)

#add_executable(tester tester.cpp)
#target_link_libraries(tester PRIVATE hole_punching_client)
