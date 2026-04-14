cmake_minimum_required(VERSION 4.2)
project(SocketSync_Chat)

set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)

add_executable(SocketSync_Chat main.cpp)

target_link_libraries(SocketSync_Chat PRIVATE Threads::Threads)
