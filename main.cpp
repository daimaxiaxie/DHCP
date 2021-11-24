#include "dhcp_message.h"

std::atomic_bool quit(false);

void UDPListen(int listenfd) {
    /*
    if (listen(listenfd, 32) != 0) {
        std::cout << "Can't listen socket!" << std::endl;
        exit(0);
    }*/

    std::cout << "UDP Listen thread (" << std::this_thread::get_id() << ") start" << std::endl;

    sockaddr_in client_addr{};
    socklen_t addr_len = sizeof(client_addr);
    char buf[1024];

    while (!quit.load()) {
        std::memset(&client_addr, 0, sizeof(client_addr));
        //int fd = accept(listenfd, (sockaddr *) &client_addr, &addr_len); // -1 : error, set errno

        //-1 : error, 0 : close / receive 0 byte
        int len = recvfrom(listenfd, buf, sizeof(buf), 0, (sockaddr *) &client_addr, &addr_len);
        //int len = sendto(listenfd, buf, sizeof(buf), 0, (sockaddr *) &client_addr, &addr_len);

        DHCPParse parse;
        const Message *msg = parse.parse(buf, len);
        //debug
        std::cout << "UDP receive " << len << " bytes" << std::endl << std::endl;

        //quit.store(true);
    }

    close(listenfd);
}

int main(int argc, char **argv) {
    Init();

    int fd = socket(AF_INET, SOCK_DGRAM, 0); // 0 auto select protocol(IPPROTO_TCP、IPPTOTO_UDP)
    int port = 68;
    sockaddr_in addr{};
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");

    int on = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_BROADCAST, &on, sizeof(on));

    //0 : success, -1 : error, set errno
    if (bind(fd, (sockaddr *) &addr, sizeof(addr)) != 0) {
        std::cout << "Can't bind socket!" << std::endl;
        exit(0);
    }

    DHCPParse parse;
    DHCPGenerate generate(fd);
    parse.clear();
    generate.Show();

    std::thread udp_listen(UDPListen, fd);

    generate.Discover();

    if (udp_listen.joinable()) {
        udp_listen.join();
    }
    Clean();
    return 0;
}
