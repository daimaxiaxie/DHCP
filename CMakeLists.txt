cmake_minimum_required(VERSION 3.16)
project(DHCP)

set(CMAKE_CXX_STANDARD 14)

find_package(Threads)

add_executable(DHCP main.cpp header.h dhcp_message.cpp dhcp_message.h)

target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
