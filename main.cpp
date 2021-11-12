#include "dhcp_message.h"

std::atomic_bool quit(false);

void UDPListen() {
    int listenfd = socket(AF_INET, SOCK_DGRAM, 0); // 0 auto select protocol(IPPROTO_TCP、IPPTOTO_UDP)
    int port = 67;
    sockaddr_in addr{};
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");

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

    std::cout << "UDP Listen thread (" << std::this_thread::get_id() << ") port(" << port << ") start" << std::endl;

    sockaddr_in client_addr{};
    socklen_t addr_len = sizeof(client_addr);
    char buf[512];

    while (!quit.load()) {
        std::memset(&client_addr, 0, sizeof(client_addr));
        //int fd = accept(listenfd, (sockaddr *) &client_addr, &addr_len); // -1 : error, set errno

        //-1 : error, 0 : close / receive 0 byte
        int len = recvfrom(listenfd, buf, sizeof(buf), 0, (sockaddr *) &client_addr, &addr_len);
        //int len = sendto(listenfd, buf, sizeof(buf), 0, (sockaddr *) &client_addr, &addr_len);

        DHCPParse parse;
        const Message *msg = parse.parse(buf, len);
        //debug
        std::cout << "UDP receive " << len << " bytes : " << *msg << std::endl;

        quit.store(true);
    }
}

int main(int argc, char **argv) {
    Init();

    DHCPParse parse;
    parse.clear();

    std::thread udp_listen(UDPListen);

    if (udp_listen.joinable()) {
        udp_listen.join();
    }
    Clean();
    return 0;
}
