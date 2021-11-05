//
// Created by lenovo on 2021/11/4.
//

#ifndef DHCP_HEADER_H
#define DHCP_HEADER_H

#include <iostream>
#include <cstring>

void Init();

#if defined(WIN32) || defined(_WIN64)
#include <WINSOCK2.h>
#pragma comment(lib, "ws2_32.lib")

void Init(){
   WSADATA wsaData;
   if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
       std::cout << "Can't initial windows socket!" << std::endl;
       exit(0);
   }
   std::cout << "DHCP Test for Windows" << std::endl;
}

#elif linux

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>

void Init() {
    std::cout << "DHCP Test for Linux" << std::endl;
}

#endif

#endif //DHCP_HEADER_H
