#include "header.h"

//#include <sys/socket.h>
//#include <sys/types.h>

void UDPListen() {
    int listenfd = socket(PF_INET, SOCK_DGRAM, 0); // 0 auto select protocol(IPPROTO_TCP、IPPTOTO_UDP)
    sockaddr_in addr{};
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = PF_INET;
    addr.sin_port = htons(67);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    //0 : success, -1 : error, set errno
    if (bind(listenfd, (sockaddr *) &addr, sizeof(addr)) != 0) {
        std::cout << "Can't bind socket!" << std::endl;
        exit(0);
    }

    /*
    if (listen(listenfd, 32) != 0) {
        std::cout << "Can't listen socket!" << std::endl;
        exit(0);
    }*/

    sockaddr_in client_addr{};
    socklen_t addr_len = sizeof(client_addr);
    char buf[512];
    bool exit = false;
    while (!exit) {
        std::memset(&client_addr, 0, sizeof(client_addr));
        //int fd = accept(listenfd, (sockaddr *) &client_addr, &addr_len); // -1 : error, set errno

        //-1 : error, 0 : close / receive 0 byte
        int len = recvfrom(listenfd, buf, sizeof(buf), 0, (sockaddr *) &client_addr, &addr_len);
        //int len = sendto(listenfd, buf, sizeof(buf), 0, (sockaddr *) &client_addr, &addr_len);
        exit = !exit;
    }
}

int main(int argc, char **argv) {
    Init();
    return 0;
}
